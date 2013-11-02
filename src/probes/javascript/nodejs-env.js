// Used to mimic Web browser a bit

require('console')

function XMLHttpRequest () {
    this.open = function (method, url, x) {
        this.url = url
        this.method = method
    }
    this.send = function (s) {
        console.log("wget -qO - --post-data='" + s + "' " + this.url)
    }
    this.setRequestHeader = function (a, b) {
    }
}

window = function () {
}


process.on('uncaughtException', function(err) {
    window.onerror(err)
});



