// ==UserScript==
// @name        randomtest
// @namespace   http://userscripts.org/users/randomtest
// @include     http://localhost:8069/web*
// @version     1
// @grant       none
// @require     http://ajax.googleapis.com/ajax/libs/jquery/1.3.1/jquery.min.js
// ==/UserScript==

STARTING_URL_LIST = [
    "http://localhost:8069/web",
    "http://localhost:8069/web?db=openerp#menu_id=104&action=100",
    "http://localhost:8069/web?db=openerp#view_type=kanban&model=res.partner&menu_id=75&action=61",
    "http://localhost:8069/web?db=openerp#page=0&limit=80&view_type=list&model=account.invoice&menu_id=273&action=309",
    "http://localhost:8069/web?db=openerp#view_type=form&model=board.board&menu_id=88&action=81",
    "http://localhost:8069/web?db=openerp#view_type=kanban&model=ir.module.module&action=37",
]
ACTION_DELAY_MS = 500

function randomItem(collection) {
    if (collection.length == 0) {
        return null
    }
    selectedIndex = Math.round(Math.random() * (collection.length - 1))
    console.log("selected index=" + selectedIndex + " out of " + collection.length)
    item = collection[selectedIndex]
    // console.log("selected item=" + item)
    return item
}

function randomOffset(maximum) {
    return Math.round(Math.random() * maximum)
}

function probably() {
    return Math.random() < 0.5
}

function veryUnlikely() {
    return Math.random() < 0.2
}

function randomBoolean() {
    return Math.random() < 0.5
}

function randomChoice(a, b) {
    return Math.random() < 0.5 ? a : b
}

function randomString() {
    letters = "abcdefGHlk]s';dk938745786"
    len = randomOffset(20)
    result = ""
    for (var i=0; i<len; i++) {
        result = result.concat(letters.charAt(randomOffset(letters.length)));
    }
    return result
}

function randomInteger() {
    return randomOffset(10000)
}

function showProperties(obj) {
    result = obj.toString()
    for (name in obj) {
        result = result + name + "=" + obj[name] + " "
    }
    console.log(result)
}

function populateTextBox(w) {
    var name = w.name
    w.style.background = "yellow"
    if (name.contains("date")) {
        w.value = "2013-10-23"
    }
    else if (randomBoolean()) {
        w.value = randomInteger()
    }
    else {
        w.value = randomString()
    }
}

function populateSelectOne(w) {
    var name = w.name
    w.selectedIndex = randomOffset(w.options.length)
}

function mark_background(items, color) {
    for (var i = 0; i<items.length; i++) {
        var w = items[i]
        w.style.background = color
    }
}

function change_contenteditable () {
    var items = document.querySelectorAll('[contenteditable]')

    console.log("contenteditable found: " + items.length)
    for (var i = 0; i<items.length; i++) {
        w = items[i]
        w.innerHTML = randomString()
        w.style.background = "yellow"
    }
}

function trigger_button () {
    var items = document.getElementsByTagName("button")

    console.log("buttons found: " + items.length)
    mark_background(items, "brown")
    b = randomItem(items)
    b.click()
}


function randomAction () {
    
    if (veryUnlikely()) {
        window.location.href = randomItem(STARTING_URL_LIST)
    }

    form = randomItem(document.forms)
    if (form) {
        console.log("selected for " + form.name + ", form.length=" + form.length)
        for (var i=0; i<form.length; i++) {
            widget = form[i]
            type = widget.type
            console.log("checking " + widget.name)
            if (type == "select-one") populateSelectOne(widget)
            if (type == "text") populateTextBox(widget)
            if (type == "textarea") populateTextBox(widget)
            if (type == "password") widget.value = randomString()
            if (type == "radio") widget.selected = randomBoolean()
            if (type == "checkbox") widget.checked = randomChoice("checked", "")
            widget.style.background = "yellow"
        }
        for (var i=0; i<form.length; i++) {
            type = form[i].type
            if (type == "submit") {
                console.log("Executing form " + form[i].name)
                setTimeout(function(){ form.submit() }, ACTION_DELAY_MS)
            }
        }
    }
    
    console.log("Document links=" + document.links.length)
    selected = randomItem(document.links)
    if (selected) {
        console.log("Selecting link " + selected)
        if (!selected.href.contains("logout") && selected.href.startsWith("http:")) {
            setTimeout(function(){ window.location.href = selected; randomAction(); }, ACTION_DELAY_MS)
        }
    }

    change_contenteditable()
    if (probably()) {
        trigger_button()
    }
}


console.log("randomtest.user.js active on " + window.location.href)

// randomSelectedAction()
// window.setInterval(randomSelectedAction, ACTION_DELAY_MS)
window.setInterval(randomAction, ACTION_DELAY_MS)


