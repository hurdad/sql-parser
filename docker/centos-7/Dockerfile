FROM centos:7

RUN yum install epel-release
RUN yum makecache
RUN yum update -y
RUN yum install -y bison flex make gcc-c++ valgrind jsoncpp-devel

CMD ["/root/sql-parser/docker/run_test.sh"]