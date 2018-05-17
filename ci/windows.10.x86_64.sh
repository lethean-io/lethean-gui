set -x
echo "CI: Windows 10 x86_64"

echo "CI: Building static release..."
./build.sh
if [ $? -ne 0 ]; then
	echo "CI: Build failed with error code: $?"
	exit 1
fi

echo "CI: Building deployable binary..."
cd build
make deploy
if [ $? -ne 0 ]; then
	echo "CI: Build failed with error code: $?"
	exit 1
fi
cd ..

echo "CI: Creating release archive..."
RELEASE_NAME="intensecoin-gui-win-64bit-$BUILD_VERSION"
cd build/release/bin/
mkdir $RELEASE_NAME
ls -alR
#cp XYZ $RELEASE_NAME/
cp ../../../ci/package-artifacts/CHANGELOG.txt $RELEASE_NAME/
cp ../../../ci/package-artifacts/README.txt $RELEASE_NAME/
zip -rv $RELEASE_NAME.zip $RELEASE_NAME
sha256sum $RELEASE_NAME.zip > $RELEASE_NAME.zip.sha256.txt
