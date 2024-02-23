import tkinter as tk
from PIL import Image, ImageTk



def toggle_play_pause():
    global is_playing

    genre = genre_dropdown.get()

    if is_playing:
        # If currently playing, switch to pause
        play_button.config(image=play_image)
        is_playing = False
        # Add functionality for pausing music here
        print("Music paused")  # Placeholder for actual pause functionality
    else:
        
        # If not playing, switch to play
        play_button.config(image=pause_image)
        bgIMG.config(image=BACKG[genre])
        is_playing = True
        # Add functionality for playing music here
        print(f"{genre}'s music playing")  # Placeholder for actual play functionality

def create_play_button(window):
    global play_image, pause_image, play_button, is_playing

    # Read the play and pause images
    play_img = Image.open("./playbutton.jpg")
    play_img = play_img.resize((100, 100))
    play_image = ImageTk.PhotoImage(play_img)

    pause_img = Image.open("./pausebutton.jpg")
    pause_img = pause_img.resize((100, 100))
    pause_image = ImageTk.PhotoImage(pause_img)

    is_playing = False

    # Create a button with play_image
    play_button = tk.Button(window, command=toggle_play_pause, image=play_image, relief="flat", bg="black", activebackground="black")
    play_button.place(x=400, y=650)

def create_genre_dropdown(window):
    global genre_dropdown

    genre_dropdown = tk.StringVar(window)
    genre_dropdown.set("開心")  # Default option
    options = ["開心", "驚悚", "熱血"]
    dropdown = tk.OptionMenu(window, genre_dropdown, *options)
    dropdown.config(width=15, height=2)  # Set the width of the dropdown menu
    dropdown.place(x=120, y=680)

def create_bgIMG(window):
    global happy_bgIMG, scary_bgIMG, power_bgIMG, bgIMG, BACKG

    happy_offset_x = 1200
    happy_offset_y = 450
    happy_bgIMG = Image.open("./happybackground.jpg")
    happy_bgIMG = happy_bgIMG.crop((0+happy_offset_x, 0+happy_offset_y, 500+happy_offset_x, 500+happy_offset_y))  # Crop the image to 500x500
    happy_bgIMG = ImageTk.PhotoImage(happy_bgIMG)

    scary_offset_x = 0
    scary_offset_y = 0
    scary_bgIMG = Image.open("./scarybackground.jpg")
    scary_bgIMG = scary_bgIMG.crop((0+scary_offset_x, 0+scary_offset_y, 500+scary_offset_x, 500+scary_offset_y))  # Crop the image to 500x500
    scary_bgIMG = ImageTk.PhotoImage(scary_bgIMG)

    power_offset_x = 0
    power_offset_y = 0
    power_bgIMG = Image.open("./powerbackground.jpg")
    power_bgIMG = power_bgIMG.crop((0+power_offset_x, 0+power_offset_y, 500+power_offset_x, 500+power_offset_y))  # Crop the image to 500x500
    power_bgIMG = ImageTk.PhotoImage(power_bgIMG)

    BACKG = {
        "開心": happy_bgIMG, 
        "驚悚": scary_bgIMG, 
        "熱血": power_bgIMG
    }

    bgIMG = tk.Label(window, image=happy_bgIMG)
    bgIMG.image = happy_bgIMG
    bgIMG.place(x=50, y=50)  # Place the image at 50, 50


def create_window():
    window = tk.Tk()
    window.title("Music Player")
    window.geometry("600x800")
    window.configure(bg='black')

    create_play_button(window)
    create_genre_dropdown(window)
    create_bgIMG(window)

    window.mainloop()

create_window()