# Docker image for GrovePI

This folder includes a base docker image that can be used to dockerize
GrovePI projects.

You can build it locally on your PI running:

```bash
$ sh build.sh
```

You can also pull it from docker hub:

```bash
$ docker pull orchestracities/grovepi
```

To use the image as basic layer for running a GrovePI application,
you can refer to the [Example](Example) folder.

The example folder dockerize the official GrovePI Led blinking script.
To test it, you need to connect a Led to Digital port 4
