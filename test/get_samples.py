# Python 3 server example
from http.server import BaseHTTPRequestHandler, HTTPServer
from os import path

from sys import argv

hostName = "127.0.0.1"
serverPort = 3000
outputPath = "~/Desktop/samples"
sampleCount = 0


class GSIServer(BaseHTTPRequestHandler):

    def do_POST(self):
        global sampleCount
        sampleCount += 1
        content_length = int(self.headers['Content-Length'])
        body = self.rfile.read(content_length).decode("utf-8")
        with open(path.join(outputPath, str(sampleCount) + ".json"), "w") as f:
            f.write(body)
        self.send_response(200, "{}")
        self.send_header("Content-type", "application/json")
        self.end_headers()
        print(body[:200] + "\n...")


if __name__ == "__main__":
    if len(argv) == 2:
        outputPath = argv[1]

    webServer = HTTPServer((hostName, serverPort), GSIServer)
    print("Server started on port %s" % serverPort)

    try:
        webServer.serve_forever()
    except KeyboardInterrupt:
        pass

    webServer.server_close()
    print("Server stopped.")
