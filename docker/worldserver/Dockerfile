FROM mediv-core:latest

USER root
RUN apt-get update
RUN apt-get install gdb -yq
COPY docker/worldserver/commands .
COPY docker/worldserver/gdbres.sh .
RUN chmod +x commands
RUN chmod +x gdbres.sh
RUN chown -R medivuser commands
RUN chown -R medivuser gdbres.sh
USER medivuser

EXPOSE 45666

#CMD ["/application/bin/worldserver", "-c", "/application/etc/worldserver.conf"]
CMD ["/application/gdbres.sh"]
