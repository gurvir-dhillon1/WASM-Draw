console.log("initing websockets")

let websocket = null
let draw_stack_interval = null
const is_production = window.location.hostname !== "localhost"
console.log("Running in:", is_production ? "production" : "local development");
console.log("Hostname:", window.location.hostname);
const default_url = "localhost:8080"
const prod_url = "wasm-draw.art"
const url = is_production ? prod_url : default_url


const open_connection = (roomCode) => {
    if (check_connection()) {
        console.log("websocket is already open")
        return
    }
    const ws_protocol = is_production ? "wss://" : "ws://"
    const ws_url = `${ws_protocol}${url}/ws?room=${roomCode}`
    
    console.log("Connecting to WebSocket URL:", ws_url)
    try {
        websocket = new WebSocket(ws_url)
    } catch (err) {
        websocket = null
        console.error(err)
    }

    websocket.onopen = () => {
        console.log("websocket connection established", url)
        showRoomCode(roomCode)
        change_join_button("join-room", true)
        const processDrawStack = async () => {
            if (check_connection()) {
                try {
                    serialized_stack = processDrawCommands()
                    let msg = {
                        type: "draw",
                        payload: serialized_stack
                    }
                    websocket.send(JSON.stringify(msg))
                } catch (err) {
                    console.error("error getting or processing draw stack", err)
                }
            }
            draw_stack_interval = setTimeout(processDrawStack, 0);
        }
        processDrawStack()
    }

    websocket.onclose = () => {
        console.log("websocket connection closed")
        hideRoomCode()
        change_join_button("join-room", false)
        clearTimeout(draw_stack_interval)
        draw_stack_interval = null
        websocket = null
    }

    websocket.onerror = (error) => {
        console.log("error: ", error)
    }

    websocket.onmessage = (event) => {
        let message = JSON.parse(event.data)
        if (message.type === "draw") {
            for (const obj of message.payload) {
                const startX = obj.startX
                const startY = obj.startY
                const endX = obj.endX
                const endY = obj.endY
                const type = obj.type
                if (type === 0) {
                    Module.ccall(
                        "drawLine",
                        null,
                        ["number", "number", "number", "number", "number"],
                        [startX, startY, endX, endY, 0]
                    )
                } else if (type === 3) {
                    Module.ccall(
                        "erase",
                        null,
                        ["number", "number", "number", "number", "number"],
                        [startX, startY, endX, endY, 0]
                    )
                }
            }
        } else if (message.type === "clear-canvas") {
            Module.ccall("clearCanvas", null, [])
        }
    }
}

const check_connection = () => {
    if (websocket && websocket.readyState === websocket.OPEN)
        return true
    else
        return false
}

const close_connection = () => {
    if (check_connection()) {
        websocket.close(1000, "closing connection normally")
    } else {
        console.log("websocket is already closed or not initialized")
    }
    websocket = null
}

const send_message = (payload = "hello") => {
    if (check_connection()) {
        websocket.send(payload)
        console.log("client sent message: ", payload)
    } else {
        console.error(`websocket is not open, message "${payload}" not sent`)
    }
}

