console.log("initializing script.js")
var Module = {
    preRun: [],
    postRun: [],
    print: function(text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ')
        console.log(text)
    },
    printErr: function(text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ')
        console.error(text)
        if (document.getElementById('output')) {
            document.getElementById('output').innerHTML += '<span style="color: red">' + text + "</span><br>"
        }
    },
    setStatus: (text) => {
        if (text) console.log(text)
    },
    canvas: (function() {
        var canvas = document.getElementById("canvas")
        canvas.addEventListener("webglcontextlost", function(e) {
            alert("webgl context lost. need to reload page")
            e.preventDefault()
        }, false)
        return canvas
    })(),
    onRuntimeInitialized: () => {
        console.log("emscripten runtime initialized")
        let width = window.innerWidth
        let height = window.innerHeight
        Module.ccall("initialize", "boolean", ["number", "number"], [width, height])
        resizeCanvas()
    }
}

function resizeCanvas() {
    let width = window.innerWidth
    let height = window.innerHeight
    Module.ccall("resizeRenderer", null, ["number", "number"], [width, height])
    if (websocket) {
        msg = {type: "draw-all"}
        websocket.send(JSON.stringify(msg))
  }
}

function setActiveButton(activeButtonId) {
    const buttons = document.querySelectorAll(".draw-options button")
    buttons.forEach(button => button.classList.remove("active"))

    if (activeButtonId) document.getElementById(activeButtonId).classList.add("active")
}

const processDrawCommands = () => {
    const newCmdCount = Module.ccall(
        "getNewCommandCount",
        "number",
        []
    )

    if (newCmdCount <= 0) return []

    const totalCmdCount = Module.ccall(
        "getDrawCommandCount",
        "number",
        []
    )

    const startXPtr = Module.ccall( "getStartXArray", "number", [])
    const startYPtr = Module.ccall( "getStartYArray", "number", [])
    const endXPtr = Module.ccall( "getEndXArray", "number", [])
    const endYPtr = Module.ccall( "getEndYArray", "number", [])
    const typePtr = Module.ccall("getDrawTypes", "number", [])

    const startXArr = new Int32Array(Module.HEAP32.buffer, startXPtr, totalCmdCount)
    const startYArr = new Int32Array(Module.HEAP32.buffer, startYPtr, totalCmdCount)
    const endXArr = new Int32Array(Module.HEAP32.buffer, endXPtr, totalCmdCount)
    const endYArr = new Int32Array(Module.HEAP32.buffer, endYPtr, totalCmdCount)
    const typeArr = new Int32Array(Module.HEAP32.buffer, typePtr, totalCmdCount)

    const cmds = []
    for (let i = totalCmdCount - newCmdCount; i < totalCmdCount; ++i) {
        cmds.push({
            startX: startXArr[i],
            startY: startYArr[i],
            endX: endXArr[i],
            endY: endYArr[i],
            type: typeArr[i]
        })
    }

    Module.ccall("markCommandsSent", null, [])

    return cmds
}

window.addEventListener("resize", resizeCanvas)

const resetZoom = () => {
    const viewportMeta = document.querySelector("meta[name='viewport']")
    if (viewportMeta) {
        // set viewport to 1:1 and disable zoom briefly
        viewportMeta.content = "width=device-width, initial-scale=1.0, maximum-scale=1.0"

        // enable zoom again after 300ms delay
        setTimeout(() => {
            viewportMeta.content = "width=device-width, initial-scale=1.0"
        }, 300);;
    }
}

const change_join_button = (button_id, joined=true) => {
    const join_button = document.getElementById(button_id)
    join_button.disabled = false
    join_button.classList.remove("not-joined", "joined")
    join_button.classList.add(joined ? "joined" : "not-joined")
}

const showRoomCode = (roomCode) => {
    const roomCodeDiv = document.getElementById("room-code")
    roomCodeDiv.textContent = `room code: ${roomCode}`
    const roomCodeContainerDiv = document.getElementById("room-code-container")
    roomCodeContainerDiv.classList.remove("hidden")
}

const hideRoomCode = () => {
    const roomCodeDiv = document.getElementById("room-code")
    roomCodeDiv.textContent = ""
    const roomCodeContainerDiv = document.getElementById("room-code-container")
    roomCodeContainerDiv.classList.add("hidden")
}

document.getElementById("join-room").addEventListener("click", () => {
    if (!websocket) {
        document.getElementById("room-modal").classList.remove("hidden")
        document.getElementById("room-code-input").focus()
    } else {
        close_connection()
        change_join_button("join-room", false)
    }
})

document.getElementById("create-room-button").addEventListener("click", async () => {
    const http_protocol = is_production ? "https://" : "http://"
    const create_url = `${http_protocol}${url}/create`
    const res = await fetch(create_url, {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        }
    })

    const data = await res.json()
    const roomCode = data.room
    open_connection(roomCode)
    document.getElementById("room-modal").classList.add("hidden")
    document.getElementById("canvas").focus()
})

document.getElementById("join-room-button").addEventListener("click", () => {
    const roomCode = document.getElementById("room-code-input").value.trim()

    if (roomCode) {
        open_connection(roomCode)
        document.getElementById("room-modal").classList.add("hidden")
        document.getElementById("canvas").focus()
    }
})

document.getElementById("close-modal-button").addEventListener("click", () => {
    document.getElementById("room-modal").classList.add("hidden")
    document.getElementById("canvas").focus()
})

document.getElementById("clear-canvas").addEventListener("click", () => {
    if (websocket) {
        const payload = JSON.stringify({ type: "clear-canvas" })
        websocket.send(payload)
        Module.ccall('clearCanvas', null, [])
    } else {
        Module.ccall('clearCanvas', null, [])
    }
})

document.getElementById("room-code-container").addEventListener("click", () => {
    const roomCode = document.getElementById("room-code").textContent.split(":")[1].trim()
    navigator.clipboard.writeText(roomCode)
    alert("Room code copied!")
})

document.getElementById("eraser").addEventListener("click", () => {
    let drawMode = Module.ccall("getDrawMode", "number", [])
    const eraseButton = document.getElementById("eraser")
    if (drawMode !== 3) {
        Module.ccall("setDrawMode", null, ["number"], [3])
    }
    else {
        Module.ccall("setDrawMode", null, ["number"], [0])
    }
    drawMode = Module.ccall("getDrawMode", "number", [])
    if (drawMode === 3)
        eraseButton.classList.add("active")
    else
        eraseButton.classList.remove("active")

})
