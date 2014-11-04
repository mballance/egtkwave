#!/bin/sh

cwd=`pwd`

export LDFLAGS='-Wl,-rpath=\$${ORIGIN}/../lib -Wl,-rpath=\$${ORIGN}'

# gtkbundle=$cwd/gtkbundle
pkgs=$cwd/pkgs
inst=$cwd/inst
final_inst=$cwd/gtkwave_linux

if test ! -d $inst; then
  mkdir -p $inst
fi

if test ! -f tcl_build; then
cd $cwd
tcldir=tcl8.5.15
if test ! -d $tcldir; then
  tar xvf $pkgs/${tcldir}-src.tar.gz
fi

rm -rf ${tcldir}_build_linux
mkdir ${tcldir}_build_linux
cd ${tcldir}_build_linux

../${tcldir}/unix/configure --prefix=$inst \
  --enable-threads 

if test $? -ne 0; then
  exit 1
fi

make 
if test $? -ne 0; then
  echo "Trying again"
  make
  if test $? -ne 0; then
    exit 1
  fi
fi

make install
if test $? -ne 0; then
  echo "Trying link again"
  make install
  if test $? -ne 0; then
    exit 1
  fi
fi

# End TCL build
cd $cwd
touch tcl_build
fi 


cd $cwd
if test ! -f tk_build; then
tkdir=tk8.5.15
if test ! -d $tkdir; then
  tar xvf $pkgs/${tkdir}-src.tar.gz
fi
rm -rf ${tkdir}_build_linux
mkdir ${tkdir}_build_linux
cd ${tkdir}_build_linux

../${tkdir}/unix/configure --prefix=$inst \
  --with-tcl=$inst/lib 


if test $? -ne 0; then
  exit 1
fi

make
if test $? -ne 0; then
  exit 1
fi

make install
if test $? -ne 0; then
  exit 1
fi

cd $cwd
touch tk_build

# End tk build
fi

cd $cwd
rm -rf xz-5.0.5
#tar xvf $pkgs/xz-5.0.5.tar.gz
#cd xz-5.0.5

#./configure --prefix=$inst 
#if test $? -ne 0; then
#  exit 1
#fi

#make
#if test $? -ne 0; then
#  exit 1
#fi

#make install
#if test $? -ne 0; then
#  exit 1
#fi

cd $cwd
gtkwave_dir=gtkwave-3.3.52

rm -rf ${gtkwave_dir}_build_linux
mkdir ${gtkwave_dir}_build_linux
cd ${gtkwave_dir}_build_linux

../${gtkwave_dir}/configure --prefix=$inst --disable-xz
if test $? -ne 0; then
  exit 1
fi

make
if test $? -ne 0; then
  exit 1
fi

make install-exec
if test $? -ne 0; then
  exit 1
fi

# This can fail
make install-data

rm -rf $final_inst
mkdir -p $final_inst

mkdir $final_inst/bin
mkdir $final_inst/lib
#cp /bin/pthreadGC2.dll $final_inst/bin
cp $inst/bin/gtkwave $final_inst/bin
cp $inst/lib/*.so $final_inst/lib

#mkdir -p $final_inst/lib/gtk-2.0/2.10.0
#cp -r $inst/lib/gtk-2.0/2.10.0/engines $final_inst/lib/gtk-2.0/2.10.0

cp -r $inst/lib/tcl8.5 $final_inst/lib
cp -r $inst/lib/tk8.5 $final_inst/lib

# cp -r $inst/manifest $final_inst

mkdir $final_inst/share
cp -r $inst/share/themes $final_inst/share

cd $final_inst/bin
#strip *.exe *.dll


