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

# Flow of Control

Before the regular computational part of the system can take place a few things happen. First, the program which calculates the photon paths -- DuDphi.cpp -- must be compiled within the root directory of the server application under the name “dudphi” without any quotes or file extension. Then when the python server application is started, the program goes through the precomputed paths that it has on file and takes every nth term from the plot list to lower the number of datapoints and increase performance on the client side (the degree of accuracy is determined by constants set at the top of the python server application). Then finally, when the python server program is up and running on the desired IP and Port (See Setup & Install) the progression of computational events created by a typical progression of a user through the workflow of the web application can begin: 

1) The user navigates to the root of the web server (i.e. localhost:5000/ -- if the user is running the server locally). In this case the python server receives the request and then returns the static homepage for the application to the user’s browser. --See the templates/ directory for details on the HTML layout. Knowledge of the Jinja HTML templating engine is required.
2) After landing on the homepage and learning about the homepage and learning about the goals of the project, the visitor then wants to navigate to the mathematics page (localhost:5000/math) to learn more about the mathematical representation of theunderlying physical system. As in the previous case, upon receiving a GET request for the page, the python server returns the static math webpage in the templates/ directory.
3) Finally, one the user has familiarized him/herself with the mathematical background, they proceed to the trajectory page (localhost:5000/getPoints) with a GET request and the python server returns to the client create.html from templates/ without modifying the page content.  
3) a) Once the user reads the on-page instructions they choose the input parameters desired and submit the HTML form by eitherpressing submit or the enter key and a http POST request is made to the same server address (localhost:5000/getPoints) the server catches the request and then looks at the contents of the HTML form.  
  -If the input parameters match one of the precomputed photon paths(i.e. perfect orbit or photon starts inside the black hole), then the server inserts the precomputed plot points and point names into the {{points}} and {{plotList}} tags of the create.html template respectively. 
    -Otherwise, python server does not know the trajectory of the photon and so a new subprocess is created wherein the server calls the pre-compiled c++ code with the starting parameters provided by the client, the resulting point list is then passed from the c++ instance to the python server via stdout, a plot list is then generated and the two lists together, points and plotList are inserted into the create.html template as per the previous case.  Finally, once the correct points and plot list are determined the python server uses the jinja templating engine to insert those values into the script tag of the getPoints page before it is returned to the user.
4) After experimenting with the web application, the user wants to delve deeper into the questions and problems revolving around the topic of black holes and behavior of light and so he/she navigates to the image correction page (localhost:5000/image) to learn more about the last portion of the project which is still in the very earliest stages of development. This another static web page which is delivered to the client in the same way as pages (1) and (2).

Each of these separate http requests (1,2,3,3a,4) are stateless and therefore can be made in any order (i.e. no particular page must be visited before another) as long as the correct HTTP request method is used (and additional form elements are provided where appropriate).  


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
Form Requirements:  The POST request form has two elements:  
--radius → Real Number in range [0,Inf)    
--sigma → Real Number in range [0,180]  
Returns:   
--GET:  HTML for the web app.  
--POST: HTML for the web app with the point data inserted directly into the script so that on page load the data is displayed automatically via the visualization script in the HEAD.

/image → Image Correction page  
Methods: GET  
Returns: HTML for the website’s image corr. page.
