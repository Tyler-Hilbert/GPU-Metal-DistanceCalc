# What the 3 GPU kernels do
1) convert_to_radians -- parses equatorial coordinate data out of string (parsed from wikipedia using getdata.py) and converts it to equatorial coordinates in radians.
2) convert_to_cartesian -- converts from equatorial to cartesian coordinate system.  
3) calculate_distances -- calculates the distance in the cartesian coordinate system.

# Install
1) Create command line xcode project
2) Add Metal and Foundation to your build
3) Downlaod metal-cpp
4) add metal-cpp/ to header search path

# Data Collection  
Python script with nested requests to scrape data from each galaxy linked on this list https://en.wikipedia.org/wiki/List_of_nearest_galaxies  
