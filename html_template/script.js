console.log("initializing script.js")
let canvas_sync = null
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
            document.getElementById('output').innerHTML += '<span style="color: red">' + text + "</span><br>";
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
        canvas_sync = new Module.getCanvasSync()
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

const testCanvasSync = (canvas_sync) => {
    const draw_stack = canvas_sync.get_rest_of_draw_stack()
    try {
        console.log("-------------------------------")
        for (let i = 0; i < draw_stack.size(); ++i) {
            let cmd = draw_stack.get(i)
            console.log(
                cmd.startX,
                cmd.startY,
                cmd.endX,
                cmd.endY,
                cmd.type
            )
        }
    } catch (err) {
        console.error("can't print size: ", err)
    }
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
