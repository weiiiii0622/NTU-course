import customtkinter
from PIL import Image
import pygame
import random
from pythonosc import udp_client

customtkinter.set_appearance_mode("Dark")
customtkinter.set_default_color_theme("dark-blue")

GREEN = ["#09D509", "#00AD00"]
GRAY = ["gray", "#646464"]
RED = ["#FF4343", "#B02D2D"]
ORANGE = ["#FFC300", "#E4AE00"]

# [path,bgm]
MUSIC_FILE = { 
    "Emo": [["./music/emo/1.wav", 100], ["./music/emo/2.wav", 100]], 
    "電子": [["./music/edm/1.wav", 175], ["./music/edm/2.wav", 175], ["./music/edm/3.wav", 175]],
    "放鬆": [["0", 75], ["1", 75]]
}

BACKGROUND = {
    "熱血": "./background/powerbackground.jpg", 
    "Emo": "./background/emobackground.jpeg", 
    "電子": "./background/edmbackground.jpeg",
    "放鬆": "./background/relaxbackground.jpeg",
    "Select a genre": "./background/defaultbackground.png"
}

class PhotoFrame(customtkinter.CTkFrame):
    def __init__(self, master):
        super().__init__(master)

        self.grid_rowconfigure(0, weight=1)
        self.grid_columnconfigure(0, weight=1)
        my_image = customtkinter.CTkImage(light_image=Image.open(BACKGROUND["Select a genre"]),
                                  dark_image=Image.open(BACKGROUND["Select a genre"]),
                                  size=(300, 300))
        self.image_label = customtkinter.CTkLabel(self, image=my_image, text="")
        self.image_label.grid(row=0, column=0, padx=10, pady=10, sticky="nswe", rowspan=2, columnspan=4)

class App(customtkinter.CTk):
    def __init__(self):
        super().__init__()

        self.title("Pd Player")
        self.geometry("550x400")
        self.grid_rowconfigure(0, weight=1)
        self.grid_rowconfigure(1, weight=0)
        self.grid_columnconfigure(0, weight=2)
        self.grid_columnconfigure((1,2,3), weight=0)

        # Relax
        self.current_client = 0
        self.client = [None for i in range(2)]
        self.client[0] = udp_client.SimpleUDPClient("127.0.0.1", 6666)
        self.client[1] = udp_client.SimpleUDPClient("127.0.0.1", 7777)

        self.prev_music = None
        self.music_file = None  # Update this with your file path
        pygame.mixer.init()

        self.isStarted = False
        self.isPlaying = False

        # Image Pulse
        self.pulse_i = 0
        self.PULSE_LAP = [3, 3.8, 7, 3, 2, 1.5, 0.6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1.2, 1.4, 1.8 , 2, 2.5]
        self.PULSE_THRESHOLD = len(self.PULSE_LAP)
        self.BPM = 100
        self.BPM = self.BPM*2 if self.BPM < 100 else self.BPM
        self.pulse_interval = int(((1 / (self.BPM / 60)) * 1000) / self.PULSE_THRESHOLD)


        self.IMAGE = {
            "熱血": [None for i in range(self.PULSE_THRESHOLD)], 
            "Emo": [None for i in range(self.PULSE_THRESHOLD)], 
            "電子": [None for i in range(self.PULSE_THRESHOLD)],
            "放鬆": [None for i in range(self.PULSE_THRESHOLD)]
        }
        self.init_pulse_img()

        self.image = customtkinter.CTkImage(light_image=Image.open(BACKGROUND["放鬆"]),
                                            dark_image=Image.open(BACKGROUND["放鬆"]),
                                            size=(300, 300))

        self.photo_frame = PhotoFrame(self)
        self.photo_frame.grid(row=0, column=0, padx=10, pady=(10, 0), sticky="nsew", columnspan=4, rowspan=2)

        self.genre_dropdown = customtkinter.CTkOptionMenu(self, values=["Select a genre", "放鬆", "Emo", "電子"],command=self.optionmenu_callback)
        self.genre_dropdown.set("Select a genre")
        self.genre_dropdown.grid(row=2, column=0, padx=10, pady=10, sticky="we", columnspan=2)

        self.play_button = customtkinter.CTkButton(self, text="Play", fg_color=GRAY[0], hover_color=GREEN[1], state="disabled", command=self.play_button_callback)
        self.play_button.grid(row=2, column=2, padx=10, pady=10, sticky="w")

        self.switch_button = customtkinter.CTkButton(self, text="Switch", fg_color=GRAY[0], hover_color=GRAY[1], state="disabled", command=self.switch_button_callback)
        self.switch_button.grid(row=2, column=3, padx=10, pady=10, sticky="w")


        # DEMO
        #self.play_button.configure(text="Play", fg_color=GREEN[0], hover_color=GREEN[1], state="normal")
        #self.switch_button.configure(fg_color=ORANGE[0], hover_color=ORANGE[1], state="normal")
        
        pygame.display.init()
        screen = pygame.display.set_mode((1, 1))
        self.music_end = pygame.USEREVENT + 1
        pygame.mixer.music.set_endevent(self.music_end)
        self.mainloop_handler()
        self.pulse_handler()
        
    def play_button_callback(self):
        if self.isStarted == False:
            print(f"Playing Genre:\"{self.genre_dropdown.get()}\" File:\"{self.music_file[0]}\"")
            self.isPlaying = True
            self.isStarted = True
            self.play_button.configure(text="Pause", fg_color=RED[0], hover_color=RED[1])
            if self.genre_dropdown.get() != "放鬆":
                pygame.mixer.music.load(self.music_file[0])
                pygame.mixer.music.play()
            else:
                self.client[self.current_client].send_message("/control", 1)

        else:
            if self.isPlaying == False:
                print(f"Resume playing {self.genre_dropdown.get()}")
                self.isPlaying = True
                self.play_button.configure(text="Pause", fg_color=RED[0], hover_color=RED[1])
                if self.genre_dropdown.get() != "放鬆":
                    pygame.mixer.music.unpause()
                else:
                    self.client[self.current_client].send_message("/control", 1)
            else:
                print("Paused.")
                self.isPlaying = False
                self.play_button.configure(text="Play", fg_color=GREEN[0], hover_color=GREEN[1])
                if self.genre_dropdown.get() != "放鬆":
                    pygame.mixer.music.pause()
                else:
                    self.client[self.current_client].send_message("/control", 0)

    def switch_button_callback(self):
        print("Switch.")
        choice = self.genre_dropdown.get()
        if(choice == "放鬆"):
            self.client[self.current_client].send_message("/control", 0)
            self.current_client = (self.current_client + 1) % 2
        else:
            available_tracks = [track for track in MUSIC_FILE[choice] if track[0] != self.prev_music]
            self.music_file = random.choice(available_tracks)
        self.prev_music = self.music_file[0]
        self.BPM = self.music_file[1]
        self.pulse_interval = int(((1 / (self.BPM / 60)) * 1000) / self.PULSE_THRESHOLD)
        self.play_button.configure(text="Play", fg_color=GREEN[0], hover_color=GREEN[1])
        self.isPlaying = False
        self.isStarted = False
        pygame.mixer.music.stop()

    def optionmenu_callback(self, choice):
        self.genre_dropdown.configure(values=["放鬆", "Emo", "電子"])
        print(f"Select Genre: {choice}")
        self.client[self.current_client].send_message("/control", 0)
        if(choice == "放鬆"):
            self.current_client = (self.current_client + 1) % 2
        available_tracks = [track for track in MUSIC_FILE[choice] if track[0] != self.prev_music]
        self.music_file = random.choice(available_tracks)
        print(self.music_file)
        self.prev_music = self.music_file[0]
        self.BPM = self.music_file[1]
        self.pulse_interval = int(((1 / (self.BPM / 60)) * 1000) / self.PULSE_THRESHOLD)
        if choice != "Select a genre":
            self.play_button.configure(text="Play", fg_color=GREEN[0], hover_color=GREEN[1], state="normal")
            self.switch_button.configure(fg_color=ORANGE[0], hover_color=ORANGE[1], state="normal")
        else:
            self.play_button.configure(text="Play", fg_color=GRAY[0], hover_color=GRAY[1], state="disabled")
            self.switch_button.configure(fg_color=GRAY[0], hover_color=GRAY[1], state="disabled")
        
        self.image = customtkinter.CTkImage(light_image=Image.open(BACKGROUND[choice]), 
                                            dark_image=Image.open(BACKGROUND[choice]),
                                            size=(300, 300))
        self.photo_frame.image_label.configure(image=self.image)
        self.isPlaying = False
        self.isStarted = False
        pygame.mixer.music.stop()

    def init_pulse_img(self):
        for genre in self.IMAGE:
            for i in range(self.PULSE_THRESHOLD):
                self.IMAGE[genre][i] = customtkinter.CTkImage(light_image=Image.open(BACKGROUND[genre]),
                                            dark_image=Image.open(BACKGROUND[genre]),
                                            size=(300+self.PULSE_LAP[i], 300+self.PULSE_LAP[i]))

    def pulse_handler(self):
        choice = self.genre_dropdown.get()
        if self.isStarted and choice != None:
            if self.isPlaying == True:
                self.pulse_i = (self.pulse_i + 1)%self.PULSE_THRESHOLD
                self.photo_frame.image_label.configure(image=self.IMAGE[choice][self.pulse_i])
            else:
                self.pulse_i = 0
                self.pulse_interval = int(((1 / (self.BPM / 60)) * 1000) / self.PULSE_THRESHOLD)
                # print(f"PULSE update by {self.pulse_interval}")
                self.photo_frame.image_label.configure(image=self.IMAGE[choice][self.pulse_i])
        self.after(self.pulse_interval, self.pulse_handler)

    def mainloop_handler(self):
        # print(self.current_client)
        for event in pygame.event.get():
            if event.type == self.music_end and self.isPlaying:
                print("REPEAT")
                pygame.mixer.music.play()

        self.after(100, self.mainloop_handler)

if __name__ == "__main__":
    app = App()
    app.mainloop()