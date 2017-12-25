FROM ubuntu:17.04

RUN apt-get update
RUN apt-get upgrade
RUN apt-get install --yes flex bison build-essential valgrind

CMD ["/root/sql-parser/docker/run_test.sh"]