console.log("initing websockets")

let websocket = null

const open_connection = () => {
    if (check_connection()) {
        console.log("websocket is already open")
        return
    }

    websocket = new WebSocket("ws://localhost:3000/ws")

    websocket.onopen = () => {
        console.log("websocket connection established")
    }

    websocket.onclose = () => {
        console.log("websocket connection closed")
    }

    websocket.onerror = (error) => {
        console.log("error: ", error)
    }

    websocket.onmessage = (event) => {
        const data = JSON.parse(event.data)
        if (data.type === "canvas_sync") {
            const img = new Image()
            img.onload = () => {
                const canvas = document.getElementById("canvas")
                const ctx = canvas.getContext("2d")
                ctx.drawImage(img, 0, 0)
            }
            img.src = data.imageData
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
}

const send_message = (payload = "hello") => {
    if (check_connection()) {
        websocket.send(payload)
        console.log("client sent message: ", payload)
    } else {
        console.error(`websocket is not open, message "${payload}" not sent`)
    }
}

const capture_and_send_canvas = () => {
    const canvas = document.getElementById("canvas")
    const canvas_data = canvas.toDataURL("image/png")

    if (check_connection()) {
        websocket.send(JSON.stringify({
            type: "canvas_sync",
            ImageData: canvas_data
        }))
    } else {
        console.error("websocket is not open")
    }
}

document.getElementById("join-room").addEventListener("click", () => {
    const join_button = document.getElementById("join-room")
    if (check_connection()) {
        join_button.textContent = "join room"
        close_connection()
    } else {
        join_button.textContent = "leave room"
        open_connection()
    }
})
