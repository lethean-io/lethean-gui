set -x

# determine build version
git describe --tags --exact-match 2> /dev/null
if [ $? -eq 0 ]; then
	BUILD_VERSION=`git describe --tags --exact-match`
else
	# replace forward slashes with underscores for feature branches
	BUILD_BRANCH=`git rev-parse --abbrev-ref HEAD | tr / _`
	BUILD_COMMIT=`git rev-parse --short HEAD`
	BUILD_VERSION="$BUILD_BRANCH-$BUILD_COMMIT"
fi
export BUILD_VERSION

# determine build host
if [ -x "$(command -v sw_vers)" ]; then
	macOSVersion=`sw_vers -productVersion`
	macOSVersion="${macOSVersion%.*}"
	macOSScript="./ci/macos.$macOSVersion.sh"
	if [ -f $macOSScript ]; then
		$macOSScript $1
	else
		echo "CI: builds not yet implemented for macOS version $macOSVersion"
	fi
elif [ -x "$(command -v lsb_release)" ]; then
	linuxDistribution=`lsb_release --id --short | awk '{print tolower($0)}'`
	linuxVersion=`lsb_release --release --short`
	linuxArchitecture=`uname -i`
	
	# if architecture is unknown try to get it from a different source
	if [ "$linuxArchitecture" = "unknown" ]; then
		linuxArchitecture=`dpkg --print-architecture`
	fi

	
	linuxScript="./ci/$linuxDistribution.$linuxVersion.$linuxArchitecture.sh"
	if [ -f $linuxScript ]; then
		$linuxScript $1
	else
		echo "CI: builds not yet implemented for $linuxDistribution version $linuxVersion $linuxArchitecture"
	fi
elif [ -x "$(command -v uname)" ]; then
	osVersion=`expr substr $(uname -s) 1 10`
	osArchitecture=`uname -m`
	if [ "$osVersion" = "MSYS_NT-10" ] && [ "$osArchitecture" = "x86_64" ]; then
		./ci/windows.10.x86_64.sh
	elif [ "$osVersion" = "MINGW64_NT" ]; then
		./ci/windows.10.x86_64.sh
	elif [ "$osVersion" = "MSYS_NT-10" ] && [ "$osArchitecture" = "i686" ]; then
		./ci/windows.10.x86.sh
	elif [ "$osVersion" = "MINGW32_NT" ]; then
		./ci/windows.10.x86.sh
	else
		echo "CI: builds not yet implemented for $osVersion $osArchitecture"
	fi
else
	echo "CI: unable to determine build system"
fi
