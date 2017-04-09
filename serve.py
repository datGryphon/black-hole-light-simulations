from flask import Flask, request, session, g, redirect, url_for, \
     abort, render_template, flash, Response, jsonify
from werkzeug.utils import secure_filename
from flask_cors import CORS, cross_origin
import subprocess
from orbit import orbit
from horizon import horizon
from horizon import plot as BHplotname
from flask import make_response
from functools import wraps, update_wrapper
from datetime import datetime

# configuration
DEBUG = True

app = Flask(__name__)
app.config.from_object(__name__)
CORS(app)

@app.route('/',methods=['GET'])
def home():
	return render_template('main.html')

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

	print session

	if request.method=='POST':

		points=horizon
		plotlist=BHplotname

		radius=request.form['radius']
		sigma=request.form['sigma']

		if not ((float(radius)==3.0 and float(sigma)==90.0) or (float(radius)<=2.0)):
			output=subprocess.check_output(["./dudphi",radius,sigma])
			photon=eval(output)
			prepost=photon.pop(-1)
			print "Process successfully called, exited with ", prepost
			points.extend(photon)
			plotlist.extend(["Photon" for i in range(len(photon))])
		elif float(radius)<=2.0:
			print "horizon"
		else:
			print "orbit"
			points.extend(orbit)
			plotlist.extend(["Photon" for i in range(len(orbit))])
		

	print len(points)
	return render_template('create.html',points=points,plotlist=plotlist,radius=radius,sigma=sigma)

# @app.after_request
# def after_request(response):
# 	response.headers['Last-Modified'] = datetime.now()
# 	response.headers['Cache-Control'] = 'no-store, no-cache, must-revalidate, post-check=0, pre-check=0, max-age=0'
# 	response.headers['Pragma'] = 'no-cache'
# 	response.headers['Expires'] = '-1'
# 	return response

if __name__ == '__main__':
	app.run(host='0.0.0.0')