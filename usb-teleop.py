import serial
import keyboard

# Establish serial connection
ser = serial.Serial('/dev/ttyACM0', 9600)

# Initialize velocity values
x_velocity = 0.0
y_velocity = 0.0
r_velocity = 0.0

# Speed settings
speed_increment = 0.05
max_speed = 0.8

# Function to send velocity values
def send_velocities():
    velocities = f"{y_velocity:.2f},{x_velocity:.2f},{r_velocity:.2f}\n"
    ser.write(velocities.encode())
    print(velocities.encode())
    # print(ser.read_all())

# Key down event handler
def on_key_down(event):
    global x_velocity, y_velocity, r_velocity

    if event.name == 'space':
        x_velocity = 0
        y_velocity = 0
        r_velocity = 0

    if event.name == 'up':
        y_velocity += speed_increment
    elif event.name == 'down':
        y_velocity += -speed_increment
    elif event.name == 'left':
        x_velocity += speed_increment
    elif event.name == 'right':
        x_velocity += -speed_increment
    elif event.name == 'shift':
        r_velocity += speed_increment
    elif event.name == 'alt':
        r_velocity += -speed_increment

    # Clamp velocity values within the range [-max_speed, max_speed]
    x_velocity = max(min(x_velocity, max_speed), -max_speed)
    y_velocity = max(min(y_velocity, max_speed), -max_speed)
    r_velocity = max(min(r_velocity, max_speed), -max_speed)

    send_velocities()

# Key up event handler
def on_key_up(event):
    global x_velocity, y_velocity, r_velocity

    if event.name in ['up', 'down']:
        y_velocity = 0.0
    elif event.name in ['left', 'right']:
        x_velocity = 0.0
    elif event.name == 'shift':
        r_velocity = 0.0

    send_velocities()

# Register key events
keyboard.on_press(on_key_down)
keyboard.on_release(on_key_up)

# Wait for key events
keyboard.wait()
