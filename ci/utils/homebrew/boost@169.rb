class BoostAT169 < Formula
  desc "Collection of portable C++ source libraries"
  homepage "https://www.boost.org/"
  url "https://downloads.sourceforge.net/project/boost/boost/1.69.0/boost_1_69_0.tar.bz2"
  sha256 "8f32d4617390d1c2d16f26a27ab60d97807b35440d45891fa340fc2648b04406"
  head "https://github.com/boostorg/boost.git", branch: "master"

  depends_on "icu4c"
  uses_from_macos "bzip2"
  uses_from_macos "zlib"

  def install
    # Force boost to compile with the desired compiler
    open("user-config.jam", "a") do |file|
      file.write "using gcc : : #{ENV.cxx} ;\n"
    end

    # libdir should be set by --prefix but isn't
    icu4c_prefix = Formula["icu4c"].opt_prefix
    bootstrap_args = %W[
      --prefix=#{prefix}
      --libdir=#{lib}
      --with-icu=#{icu4c_prefix}
      --with-toolset=gcc
    ]

    # Handle libraries that will not be built.
    #


    with_libraries = ["atomic", "system", "filesystem", "thread", "date_time", "chrono",
                        "regex", "serialization", "program_options", "locale" ]


    # Boost.Log cannot be built using Apple GCC at the moment. Disabled
    # on such systems.
    without_libraries << "log" if ENV.compiler == :gcc

    bootstrap_args << "--with-libraries=#{with_libraries.join(",")}"

    # layout should be synchronized with boost-python and boost-mpi
    args = %W[
      --prefix=#{prefix}
      --libdir=#{lib}
      -j#{ENV.make_jobs}
      --user-config=user-config.jam
      install
    ]


    system "./bootstrap.sh", *bootstrap_args
    system "./b2", "headers"
    system "./b2", *args
  end

  def caveats
    s = ""
    # ENV.compiler doesn't exist in caveats. Check library availability
    # instead.
    if Dir["#{lib}/libboost_log*"].empty?
      s += <<~EOS
        Building of Boost.Log is disabled because it requires newer GCC or Clang.
      EOS
    end

    s
  end

  test do
    (testpath/"test.cpp").write <<~EOS
      #include <boost/algorithm/string.hpp>
      #include <string>
      #include <vector>
      #include <assert.h>
      using namespace boost::algorithm;
      using namespace std;

      int main()
      {
        string str("a,b");
        vector<string> strVec;
        split(strVec, str, is_any_of(","));
        assert(strVec.size()==2);
        assert(strVec[0]=="a");
        assert(strVec[1]=="b");
        return 0;
      }
    EOS
    system ENV.cxx, "test.cpp", "-std=c++14", "-stdlib=libc++", "-o", "test"
    system "./test"
  end
end