FROM qvipka/ubuntu-boost-kafka:latest

RUN mkdir /sources && mkdir /application

# Копируем файлы для компиляции и sql для updates
WORKDIR /sources
COPY .git/ .git/
COPY cmake/ cmake/
COPY sql/ sql/
COPY dep/ dep/
COPY src/ src/
COPY CMakeLists.txt .
COPY revision_data.h.in.cmake .

# Компилим сорсы
RUN mkdir build
WORKDIR /sources/build
RUN pwd
RUN cmake ../ -DCMAKE_INSTALL_PREFIX=/application -DTOOLS=0 -DWITH_WARNINGS=1
RUN make -j 12
RUN make install

# Удаляем из сорсов всё, кроме sql (нужен для updates)
WORKDIR /sources
RUN rm -r build
RUN rm -r .git
RUN rm -r cmake
RUN rm -r dep
RUN rm -r src
RUN rm CMakeLists.txt
RUN rm revision_data.h.in.cmake

# Переходим в рабочий каталог
WORKDIR /application
RUN mkdir logs
RUN mkdir data

RUN useradd -ms /bin/bash medivuser
RUN chown -R medivuser /application
RUN chown -R medivuser /sources
USER medivuser