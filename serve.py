from flask import Flask, request, session, g, redirect, url_for, \
     abort, render_template, flash, Response, jsonify, make_response
from werkzeug.utils import secure_filename
import subprocess
from orbit import orbit
from horizon import horizon
from horizon import plot as BHplotname
from os import path

#########ADMIN DEFINED CONSTANTS##############
ORBIT_STEP=1
HORIZ_STEP=100
BH_STEP=100
PHOTON_STEP=100
#default HOST is '0.0.0.0' broadcasts on all available IP's including loopback
HOST='localhost'	
#5000 is default for development purposes... Ports 0 to 1023 are reserved for the system
PORT=5000
##############################################

DUDPHI=path.join(path.split(path.abspath(__file__))[0],"dudphi")

rng=range(0,len(orbit),ORBIT_STEP)
rng.append(-1)
orbit = [orbit[i] for i in rng]

orbitplotname = ["Photon" for i in range(len(orbit))]

rng=range(0,len(horizon),HORIZ_STEP)
rng.append(-1)
horizon = [horizon[i] for i in rng]

rng=range(0,len(BHplotname),BH_STEP)
rng.append(-1)
BHplotname = [BHplotname[i] for i in rng]

# configuration ----------Comment out the following line before deployment
# DEBUG = True

app = Flask(__name__)
app.config.from_object(__name__)

@app.route('/',methods=['GET'])
def home():
	return render_template('main.html')

@app.route('/math',methods=['GET'])
def math():
	return render_template('math.html')

@app.route('/input',methods=['GET'])
def input():
	return render_template('input.html')

@app.route('/image',methods=['GET'])
def image():
	return render_template('image.html')

@app.route('/getPoints',methods=['GET','POST'])
def getPoints():
	points=[]
	photon=[]
	radius="5.0"
	sigma="90.0"
	plotlist=[]

	if request.method=='POST':

		radius=float(request.form['radius'])
		sigma=float(request.form['sigma'])

		if not((radius==3.0 and sigma==90.0) or (radius<=2.0)):

			output=subprocess.check_output([DUDPHI,str(radius),str(sigma)])
			photon=eval(output)
			prepost=photon.pop(-1)
			print "Process successfully called, exited with ", prepost
			r= range(0,len(photon),PHOTON_STEP)
			r.append(-1)
			points= [ photon[i] for i in r]
			plotlist = ["Photon" for i in range(len(points))]
			points.extend(horizon)
			plotlist.extend(BHplotname)
		elif sigma==0:
			print "sigma = 0 "
		elif float(radius)<=2.0:
			print "horizon"
			points = horizon
			plotlist = BHplotname
		else:
			print "orbit"
			points = orbit
			points.extend(horizon)
			plotlist = orbitplotname
			plotlist.extend(BHplotname)
		
	return render_template('create.html',points=points,plotlist=plotlist,radius=radius,sigma=sigma)

if __name__ == '__main__':
	app.run(host=HOST,port=PORT)