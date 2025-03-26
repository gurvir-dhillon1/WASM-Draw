console.log("initing websockets")

const websocket = new WebSocket("ws://localhost:3000/ws")

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
    console.log("message received from server: ", event.data)
}

const send_message = (payload = "hello") => {
    if (websocket.readyState === websocket.OPEN) {
        websocket.send(payload)
        console.log("client sent message: ", payload)
    } else {
        console.error(`websocket is not open, message "${payload}" not sent`)
    }
}
