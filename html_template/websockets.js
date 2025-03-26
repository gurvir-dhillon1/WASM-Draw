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
        console.log("message received: ", event.data)
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
