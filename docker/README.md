# Usage

## Prepare source
COPY ``docker/docker-compose-example.yml`` to ``docker/docker-compose.yml`` and change on your volumes and etc.

> cp docker/docker-compose-example.yml docker/docker-compose.yml

## Commands:

### Compilation fresh version
> docker build -f docker/core/Dockerfile --no-cache --progress=plain -t mediv-core .

### Build authserver
> docker build -f docker/authserver/Dockerfile --no-cache --progress=plain -t mediv-authserver .

### Build worldserver
> docker build -f docker/worldserver/Dockerfile --no-cache --progress=plain -t mediv-worldserver .

### Run services
> docker-compose -f docker/docker-compose.yml up -d

### Stop services
> docker-compose -f docker/docker-compose.yml down