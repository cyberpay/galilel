/**
 * Copyright (c) 2015-2018 COLX Developers
 * Copyright (c) 2018-2019 Galilel Developers
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "bootstrap/ziputil.h"
#include "utiltime.h"
#include "tinyformat.h"

#include <string>
#include <fstream>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/operations.hpp>

static bool CompareFiles(const std::string& left, const std::string& right)
{
    std::ifstream f1(left), f2(right);
    std::string str1((std::istreambuf_iterator<char>(f1)), std::istreambuf_iterator<char>());
    std::string str2((std::istreambuf_iterator<char>(f2)), std::istreambuf_iterator<char>());
    return str1 == str2;
}

BOOST_AUTO_TEST_SUITE(minizip_tests)

BOOST_AUTO_TEST_CASE(zip_unzip_test)
{
    std::string err;
    const std::string datadir("test/data");
    const std::string unzipdir("data_unzip");
    const std::string zipfile("data.zip");

    BOOST_REQUIRE(boost::filesystem::exists(datadir));

    /** invalid data dir */
    BOOST_CHECK_MESSAGE(!ZipCreate(zipfile, "not-exist-dir", err), err);

    /** create zip */
    BOOST_REQUIRE_MESSAGE(ZipCreate(zipfile, datadir, err), err);
    time_t time1 = boost::filesystem::last_write_time(zipfile);
    MilliSleep(1000); /** time1 resolution is seconds, so wait to get enough difference */

    /** overwrite zip */
    BOOST_REQUIRE_MESSAGE(ZipCreate(zipfile, datadir, err), err);
    time_t time2 = boost::filesystem::last_write_time(zipfile);

    BOOST_CHECK(time2 > time1);

    if (boost::filesystem::exists(unzipdir))
        boost::filesystem::remove_all(unzipdir);
    boost::filesystem::create_directory(unzipdir);

    const std::string fOriginal = strprintf("%s/README.md", datadir);
    const std::string fUnzipped = strprintf("%s/%s/README.md", unzipdir, datadir);

    /** extract */
    BOOST_REQUIRE_MESSAGE(ZipExtract(zipfile, unzipdir, err), err);
    BOOST_REQUIRE_MESSAGE(CompareFiles(fOriginal, fUnzipped), "files are not equal");

    /** modify unzipped file */
    std::ofstream readme(fUnzipped, std::ios_base::app);
    readme << "some content";
    readme.close();
    BOOST_REQUIRE(!CompareFiles(fOriginal, fUnzipped));

    /** extract overwrite */
    BOOST_REQUIRE_MESSAGE(ZipExtract(zipfile, unzipdir, err), err);
    BOOST_REQUIRE_MESSAGE(CompareFiles(fOriginal, fUnzipped), "files are not equal");

    /** invalid input file */
    BOOST_CHECK_MESSAGE(!ZipExtract("not-exist-file", unzipdir, err), err);

    /** invalid output dir */
    BOOST_CHECK_MESSAGE(!ZipExtract(zipfile, "not-exist-dir", err), err);

    boost::filesystem::remove(zipfile);
    boost::filesystem::remove_all(unzipdir);
}

BOOST_AUTO_TEST_SUITE_END()
