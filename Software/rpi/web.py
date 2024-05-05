from flask import Flask, request, render_template

app = Flask(__name__)





@app.route('/', methods=['GET', 'POST'])
def index():
    return render_template('index.html')


@app.route('/text', methods=['GET', 'POST'])
def text():
    lines = ["", ""]
    if request.method == 'POST':
        lines = [request.form.get('line1'), request.form.get('line2')]
        print(lines)
    return render_template("text.html", text=lines)


if __name__ == '__main__':
    app.run('127.0.0.1', port=8080, debug=True)
