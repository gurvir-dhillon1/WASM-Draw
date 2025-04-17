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
        resizeCanvas()
    }
}

function resizeCanvas() {
    let width = window.innerWidth
    let height = window.innerHeight
    Module.ccall("resize_renderer", null, ["number", "number"], [width, height])
}

function setActiveButton(activeButtonId) {
    const buttons = document.querySelectorAll(".draw-options button")
    buttons.forEach(button => button.classList.remove("active"))

    if (activeButtonId) document.getElementById(activeButtonId).classList.add("active")
}

const onLineClick = () => {
    const lineMode = Module.ccall("set_line_mode", "number", [])
    console.log("Line Mode:", lineMode)
    setActiveButton("line-mode-button")
}

const onCircleClick = () => {
    const circleMode = Module.ccall("set_circle_mode", "number", [])
    console.log("Circle Mode:", circleMode)
    setActiveButton("circle-mode-button")
}

const onSquareClick = () => {
    const squareMode = Module.ccall("set_square_mode", "number", [])
    console.log("Square Mode:", squareMode)
    setActiveButton("square-mode-button")
}

const getLastMousePosition = () => {
    const x = Module.ccall("get_last_mouse_x", "number", [])
    const y = Module.ccall("get_last_mouse_y", "number", [])
    return [x, y]
}

const processDrawCommands = () => {
    const newCmdCount = Module.ccall(
        "get_new_command_count",
        "number",
        []
    )

    if (newCmdCount <= 0) return []

    const totalCmdCount = Module.ccall(
        "get_draw_command_count",
        "number",
        []
    )

    const startXPtr = Module.ccall( "get_start_x_array", "number", [])
    const startYPtr = Module.ccall( "get_start_y_array", "number", [])
    const endXPtr = Module.ccall( "get_end_x_array", "number", [])
    const endYPtr = Module.ccall( "get_end_y_array", "number", [])
    const typePtr = Module.ccall("get_draw_types", "number", [])

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

    Module.ccall("mark_commands_sent", null, [])

    return cmds
}

document.getElementById("canvas").addEventListener("mouseup", () => {
    setTimeout(() => {
        const lineMode = Module.ccall("get_line_mode", "number", [])
        const circleMode = Module.ccall("get_circle_mode", "number", [])
        const squareMode = Module.ccall("get_square_mode", "number", [])

        if (lineMode == 0 && circleMode == 0 && squareMode == 0) {
            setActiveButton(null)
        }
    }, 100)
})

window.addEventListener("resize", resizeCanvas)

const change_join_button = (button_id, joined=true) => {
    const join_button = document.getElementById(button_id)
    join_button.disabled = false
    join_button.classList.remove("not-joined", "joined")
    join_button.classList.add(joined ? "joined" : "not-joined")
}

const showRoomCode = (roomCode) => {
    const roomCodeDiv = document.getElementById("room-code")
    roomCodeDiv.textContent = `room code: ${roomCode}`
    roomCodeDiv.classList.remove("hidden")
}

const hideRoomCode = () => {
    const roomCodeDiv = document.getElementById("room-code")
    roomCodeDiv.textContent = ""
    roomCodeDiv.classList.add("hidden")
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
    const res = await fetch("http://localhost:8080/create")

    const data = await res.json()
    console.log(data)
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
