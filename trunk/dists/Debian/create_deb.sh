#!/bin/sh

set -e -x

#1/Extract sources
src_base_dir=quteqoin-read-only
if [ -z "$(svn info . | grep URL | grep "dists/Debian")" ]
then
	svn checkout http://quteqoin.googlecode.com/svn/trunk/ ${src_base_dir}
	cd ${src_base_dir}
else
	cd ../../
	src_base_dir=$(pwd | awk -F '/' '{print $NF;}')
fi
	

#2/prepare packaging
svn_ver=$(svnversion .)

cp -R dists/Debian debian
dch -v 0.0$svn_ver "plop" #create entry in the changelog
dch -r -
cd ..

tar_name=$(printf "quteqoin_0.0%d.orig.tar.gz" "$svn_ver")
tar zcf ${tar_name} ${src_base_dir}

#3/build package
cd ${src_base_dir}
dpkg-buildpackage -us -uc

#4/echo result
cd ..
echo "Your package is available in ${PWD}"
