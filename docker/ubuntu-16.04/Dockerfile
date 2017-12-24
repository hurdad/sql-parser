FROM ubuntu:16.04

RUN apt-get update
RUN apt-get upgrade
RUN apt-get install --yes make flex bison

CMD ["/root/sql-parser/docker/run_test.sh"]