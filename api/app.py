#!/usr/bin/env python3 

from flask import Flask,request,jsonify,render_template
import tensorflow as tf
import numpy as np
import pandas as pd
from sklearn.preprocessing import StandardScaler

app = Flask(__name__,static_url_path='', 
            static_folder='static',
            template_folder='templates')


def prepare(vector):
    data = vector
    Fs=700
    T=1/Fs

    try:
        x=[] #signal vector
        y=[] #label vector : asthma or no asthma
        dataa= np.array(data[0], dtype=np.float32)
        zvals=dataa
        print(zvals)
        L=len(zvals)
        N=len(zvals)
        zvals=zvals.astype(int)
        moy=sum(zvals)/len(zvals)
        zvals=zvals-moy
        fftvals=np.fft.fft(zvals,L) #computing the fft 
        
        SSB = fftvals[1:int(N/2)+1]
        SSB[2:len(SSB)] = 2*SSB[2:len(SSB)]
        f = [i*(Fs/N) for i in range(0,int(N/2))]
        freq=np.array(f)
        indices=freq<35 #filtering the signal using a low pass filter
        clean=SSB*indices
        ffiltz=np.fft.ifft(clean,L) #inversing the fft
        finalvals=np.real(ffiltz) #filtered values
        
        #normalization
        df=pd.DataFrame({'Z':finalvals})
        scaler=StandardScaler()
        scaler.fit(df)
        StandardScaler(copy=True)
        z_scaled_data=scaler.transform(df)
        x.append(z_scaled_data.T[0])
        x=np.array(x)
        
        return (x)

    except Exception as e:
        print(e)

@app.route("/predict", methods = ["POST"])
def predict():

    try:
        input_json = request.get_json(force=True) 
        data = input_json['vector']

        #prepare data
        x=prepare(data)
        
        #prediction
        model= tf.keras.models.load_model('model.h5')
        prediction = model.predict(x)
        
        if (prediction[0][0].round() ==1):
            Category= 'Asthma'
        else:
            Category= 'Not Asthma'
            
        return jsonify({'prediction' : str(float(prediction[0][0])), 'Category ': Category })

    except Exception as e:
        print(e)

@app.route('/')
def index():
    return render_template('index.html')

if __name__ == '__app__':
    app.run()
