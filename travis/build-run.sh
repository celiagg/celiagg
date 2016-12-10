#!/bin/bash

# Run the build-wheels script in the docker container
docker run --rm -v `pwd`:/io $DOCKER_IMAGE /io/travis/build-wheels.sh
