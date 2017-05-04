# black-hole-light-simulations

This repo provides serverside code for a web application which helps teach and explain to students how light particles behave near black holes. 

# system req & install 

This setup section assumes that the target platform for running the server application is a x86_64 machine using a debian based GNU/Linux distribution on the 4.4.0-75-generic kernel which comes has the GNU g++ compiler, python 2.7.12, PIP python package installer, and the apt package manager installed already and assumes that the administrator has root privileges on the machine. 

Use apt to install the required libraries. 


`$sudo apt install libgsl0ldbl libgsl0-dev gsl-bin git`

  2) Install the Flask library for python 2.7.12.

`$sudo pip install Flask`

  3) Use git to clone the project code to the desired install directory. One should use sudo on the second command if writing to a protected directory.

`$cd to/desired/location/`

`$git clone https://github.com/datGryphon/black-hole-light-simulations.git`

  4) Use g++ and the GNU scientific library to compile the point generating program from within the project directory. One should use sudo on the second command if writing to a protected directory.

`$cd black-hole-light-simulations`

`$g++ DuDphi.cpp -lgsl -lgslcblas -lm -g -Wall -o dudphi`

  5) OPTIONAL: Change the HOST and PORT constants at the top of serve.py with the help of your favorite text editor by changing lines 15 and 17. If the server should run on a specific static IP address change line 15 to…
    HOST='MY.IP.AD.DR'
Or if the server should run on all outgoing IP addresses including localhost, change line 15 to… 
    HOST='0.0.0.0'
Then if the default IP port(5000) is taken or otherwise undesirable, change line 17 replacing #### with a valid integer in the range [1024,65535]
    PORT=####

  6) Finally, provided that no errors or warnings have arisen, the program is ready to run.

`$python path/to/project/serve.py`

While the server process is running, the web application can be accessed by navigating to http://MY.IP.AD.DR:PORT/ in a browser which is connected to the same network. If the server should be visible from outside the network, the network firewall will need to be reconfigured to forward the running port of the static IP of the machine to the network’s public IP address. If the server should run on machine startup, then the administrator should follow the following guide to configure Apache2 to handle and relay http requests for Flask. 
http://flask.pocoo.org/docs/0.12/deploying/mod_wsgi/#configuring-apache 

# Application Program Interface

The python server creates four separate http request access points which are defined as follows. 

/ → Rootpage of Site
Methods: GET
Returns: HTML for the website’s front page.|

/math → Mathematics page
Methods: GET
Returns: HTML for the website’s math page.

/getPoints → Trajectory page and Web-App
Methods: GET,POST
Form Requirements:   
  The POST request form has two elements:
    radius → Real Number in range [0,Inf)
     sigma → Real Number in range [0,180]
Returns: 
  GET:  HTML for the web app.
  POST: HTML for the web app with the point data inserted directly into the <script> so that on page load the data is displayed automatically via the visualization script in the <HEAD>.

/image → Image Correction page
Methods: GET
Returns: HTML for the website’s image corr. page.

