FROM debian:testing
RUN echo deb-src http://deb.debian.org/debian testing main >> /etc/apt/sources.list
RUN apt-get update && apt-get -y build-dep pekwm && apt-get -y install curl
COPY . /src/pekwm/
WORKDIR /src/pekwm
RUN curl -L -f http://deb.debian.org/debian/pool/main/p/pekwm/pekwm_0.1.17-3.debian.tar.xz | tar xvJ
RUN ./autogen.sh
RUN ./configure
RUN make -j

ARG date
RUN printf 'pekwm (0.1.17-3.%s) unstable; urgency=medium\n\n  * auto build\n\n -- Docker Builder <none@none.none>  %s' "$(date --date="$date" +%s)" "$(date --date="$date" --utc -R)" > debian/tmp && \
    cat debian/changelog >> debian/tmp && \
    mv debian/tmp debian/changelog

RUN debian/rules binary
