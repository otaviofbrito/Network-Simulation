# :satellite: Network Simulation

![GitHub Actions](https://github.com/otaviofbrito/Network-Simulation/actions/workflows/makefile.yml/badge.svg)

:page_with_curl: Checkout [Github Pages](https://otaviofbrito.github.io/Network-Simulation/report.html).


### Pre-Requisites
These tools are often pre-installed on your OS. If not, visit their websites and install them:
- [Python 3](https://www.python.org/downloads/source/)
- [pip](https://pypi.org/project/pip/)
- [GCC](https://gcc.gnu.org/install/)
- [GNU Make](https://www.gnu.org/software/make/)

### Usage
The project can be easily built using a Makefile.

At the root directory, run the following commands:
```shell
# Build project and install dependencies
make build
```

After the build process, you can run the project:

```shell
# Run project
make
```

Once executed, a report will automatically open, displaying the graphical visualizations.

```shell
# Clean objects, reports and generated data
make clean
```

You can also run it with predefined inputs in the input.txt file:
```shell
# Express run with input.txt
make auto
```

To fully clean the environment, run:
```shell
# Remove generated data, objects and virtual environment
make purge
```
