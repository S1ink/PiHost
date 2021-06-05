import functions as fn
from flask import Flask, render_template, request, redirect

app = Flask(__name__)
@app.route('/')
def main():
    temp = str(fn.info.cputemp())
    util = str(fn.info.cpusage())
    templateData = {
        'title' : 'Welcome?',
        'temp' : temp,
        'util' : util
    }
    return render_template('template.html', **templateData)

# @app.route('/signup', methods = ['POST'])
# def signup():
#     email = request.form['email']
#     print(email)
#     return redirect('/')

if __name__ == "__main__":
    app.run(debug=True, port=80, host='0.0.0.0')