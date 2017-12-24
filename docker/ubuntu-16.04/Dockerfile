FROM centos:7

RUN yum update -y
RUN yum install -y bison flex gcc-c++

CMD ["/root/sql-parser/docker/run_test.sh"]