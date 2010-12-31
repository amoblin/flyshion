#!/bin/sh

. ./tarballfunc.sh

create_tmp_dir

process_version

gen_tarball

gen_standalone_tarball


sha256sum_gpg

if [ "$1" != "test" ];then
	exit 0
fi

do_test
