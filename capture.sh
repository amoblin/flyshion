#!/bin/bash
#written by @levin108
#This script is used to capture the packets of fetion client,and help to anlysis fetion protocol.
#You need to have openfetion installed in your computer,or maybe libfetion,I don't know,whatever...

if [ $UID -ne 0 ]; then
	echo "perminission denied,you must be root to run this script"
	exit 1
fi

if [ $# == 0 ]; then
	echo -n "please input the mobile number:"
	read mobileno
else
	mobileno=$1
fi

if [ ${#mobileno} -ne 11 ];then
	echo "wrong mobile number"
	exit 1
fi

protocol_version="4.0.2510"
config_uri="http://nav.fetion.com.cn/nav/getsystemconfig.aspx"
config_body="<config><user mobile-no=\"$mobileno\"/> \
			<client type=\"PC\" version=\"$protocol_version\" \
			platform=\"W5.1\"/><servers version=\"0\"/> \
			<parameters version=\"0\"/><hints version=\"0\"/></config>"

echo "Getting fetion sipc servr address..."

config_xml=`curl -d "$config_body" -A "IIC2.0/PC $protocol_version" $config_uri 2> /dev/null`
proxy_endpoint=`echo $config_xml | sed 's/.*<sipc-proxy>\([^<]*\).*/\1/'`
proxy_ip=`echo $proxy_endpoint | sed 's/:[0-9]*$//'`
proxy_port=`echo $proxy_endpoint | sed 's/.*://'`

echo "Sipc server address : $proxy_ip:$proxy_port"
echo "Start capturing,now start your fetion client and login."
echo

tcpdump -n -A -l -t -s 0 tcp and host $proxy_ip \
	and port $proxy_port or host nav.fetion.com.cn 2>/dev/null | awk '
/^IP/{
	if($NF == 0) {
		is_data_seg = 0;
	} else {
		printf("\n%s ----> %s\n", $2, $4)
		printf("-----------------------------------\n\n")
		# whether it is a data segment
		is_data_seg = 1;
		# eth/ip/tcp header length
		bytes_before_protocol = 52;
	}
}

!/^IP /{
	if(is_data_seg) {
		if(length($0) < bytes_before_protocol) {
			bytes_before_protocol -= length($0)
			next;
		}
		if(length($0) >= bytes_before_protocol && 
			bytes_before_protocol != 0) {
			$0 = substr($0, bytes_before_protocol);
			bytes_before_protocol = 0;
		}
		print $0
	}
}
'
