# What the 2 GPU kernels do
1) parse_convert_to_cartesian -- parses equatorial coordinate data out of string (scraped from wikipedia using getdata.py) and converts it to equatorial coordinates in radians. Then converts from equatorial to cartesian coordinate system. 
2) calculate_distances -- calculates the distance in the cartesian coordinate system.

# Install
1) Create command line xcode project
2) Add Metal and Foundation to your build
3) Download metal-cpp -- https://github.com/bkaradzic/metal-cpp  
4) add metal-cpp/ to header search path

# Data Collection  
Python script with nested requests to scrape data from each galaxy linked on this list https://en.wikipedia.org/wiki/List_of_nearest_galaxies  

# Project Precursor
CUDA implementation -- https://github.com/Tyler-Hilbert/SpaceTripPlanner
