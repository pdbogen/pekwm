#!/bin/sh

date="$(date +%s)"
docker build --build-arg date=@$date -t builder .
docker run builder cat /src/pekwm_0.1.17-3.${date}_amd64.deb > pekwm_0.1.17-3.${date}_amd64.deb
