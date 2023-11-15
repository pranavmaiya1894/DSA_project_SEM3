#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

char *AllSongs[] = {
    "jawan.mp3",
    "heeriye.mp3",
    "JoTumNaHo.mp3",
    "KabhiJoBadalbarse.mp3",
    "kesariya.mp3",
    "raam_siya_raam.mp3",
    "terehawaale.mp3"
};

char *musicStats[]={
    "song name :jawan\nartist name:arjit singh\nduration:00:03:20\nGenre: Bollywood\n",
    "song name :heeriye\nartist name:arjit singh\nduration:00:03:19\nGenre: Lofi\n",
    "song name :JoTumNaHo\nartist name:arjit singh\nduration:00:04:08\nGenre: Bollywood\n",
    "song name :KabhiJoBadalbarse\nartist name:arjit singh\nduration:00:04:14\nGenre: Bollywood\n",
    "song name :kesariya\nartist name:arjit singh\nduration:00:04:28\nGenre: Club\n",
    "song name :raam_siya_raam\nartist name:arjit singh\nduration:00:03:50\nGenre: Devotional\n",
    "song name :terehawaale\nartist name:arjit singh\nduration:00:05:50\nGenre: Club\n"

};

static int SongPlayCounts[7];

typedef struct Node {
    char *songName;
    struct Node *next;
} Node;

typedef struct Queue {
    Node *front;
    Node *rear;
} Queue;

void print_menu() {
    printf("Press 1 to play all the songs in your device\n");
    printf("Press 2 to add songs to your playlist\n");
    printf("Press 3 to delete songs from your playlist\n");
    printf("Press 4 to play your playlist\n");
    printf("Press 5 to print the songs in your playlist\n");
    printf("Press 6 to add songs to your queue\n");
    printf("Press 7 to play a random song from your local storage\n");
    printf("Press 8 to play your most played song\n");
    printf("Press 9 to search and play for a particular song\n");
    printf("Press 10 to search for a particular song's statistics\n");
}

Queue *initializeQueue() {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (queue == NULL) {
        exit(1);
    }
    queue->front = queue->rear = NULL;
    return queue;
}

void playSong(const char *song) { char command[100];
    snprintf(command, sizeof(command), "mplayer %s", song);
    system(command);
    int songIndex = -1;
    for (int i = 0; i < 7; i++) {
        if (strcmp(AllSongs[i], song) == 0) {
            songIndex = i;
            break;
        }
    }
    if (songIndex >= 0) {
        SongPlayCounts[songIndex]++;
    }
}

void insertSong(Node **head, char *songName) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->songName = strdup(songName);
    newNode->next = NULL;

    if (*head == NULL) {
        newNode->next = newNode;
        *head = newNode;
    } else {
        Node *lastNode = *head;
        while (lastNode->next != *head) {
            lastNode = lastNode->next;
        }

        lastNode->next = newNode;
        newNode->next = *head;
    }
}

void playAllSongs() {
    for (int i = 0; i < 7; i++) {
        playSong(AllSongs[i]);
        system("clear");
        printf("******\n\n");
        sleep(3);
    }
}

void freePlaylist(Node *head) {
    if (head == NULL) {
        return;
    }
    Node *current = head->next;
    while (current != head) {
        Node *temp = current;
        current = current->next;
        free(temp->songName);
        free(temp);
    }
    free(head->songName);
    free(head);
}

void deleteSong(Node **head, char *songName) {
    if (*head == NULL) {
        printf("Playlist is empty.\n");
        return;
    }
    Node *current = (*head)->next;
    Node *prev = *head;
    while (current != *head) {
        if (strcmp(current->songName, songName) == 0) {
            prev->next = current->next;
            free(current->songName);
            free(current);
            printf("Song deleted from your playlist.\n");
            return;
        }
        prev = current;
        current = current->next;
    }
    if (strcmp(current->songName, songName) == 0) {
        free(current->songName);
        free(current);
        printf("Song deleted from your playlist.\n");
        *head = NULL;
    } else {
        printf("%s not found in the playlist.\n", songName);
    }
}

void playPlaylist(Node *head) {
    if (head == NULL) {
        printf("Playlist is empty.\n");
    } else {
        Node *current = head;
		playSong(current -> songName);
		current = current -> next;
        system("clear");
        printf("******\n\n");
        sleep(3);
		while (current != head) {
            playSong(current->songName);
            current = current->next;
            system("clear");
            printf("******\n\n");
            sleep(3);
        }
        printf("End of playlist. You have 10 seconds to exit from the current playlist mode :\n");
        struct timeval tv;
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        int ret = select(1, &fds, NULL, NULL, &tv);
        if (ret == -1) {
            perror("select");
        } else if (ret == 0) {
            printf("\nNo input received within the timeout. Restarting the playlist.\n");
            playPlaylist(head);
        } else {
            int choice;
            scanf("%d", &choice);
        }
    }
}

Queue *createQ() {
    Queue *playlist = initializeQueue();
    printf("The below shows the list of songs available, select multiple songs and add them to your queue\n");
    for (int i = 0; i < 7; i++) {
        printf("%d. %s\n", i + 1, AllSongs[i]);
    }
    printf("Enter the songs that you want to add to your queue\nEnter -1 to stop adding\n");
    int choice;
    while (1) {
        scanf("%d", &choice);
        if (choice == -1) {
            break;
        }
        if (choice >= 1 && choice <= 7) {
            Node *newNode = (Node *)malloc(sizeof(Node));
            newNode->songName = AllSongs[choice - 1];
            newNode->next = NULL;
            if (playlist->rear == NULL) {
                playlist->front = playlist->rear = newNode;
            } else {
                playlist->rear->next = newNode;
                playlist->rear = newNode;
            }
        } else {
            printf("Invalid choice. Please select a valid song index.\n");
        }
    }
    return playlist;
}

void playQ(Queue *playlist) {
    Node *current = playlist->front;
    while (current != NULL) {
        playSong(current->songName);
        current = current->next;
        system("clear");
        printf("******\n\n");
        sleep(3);
    }
}

void playRandomSong() {
    int numSongs = sizeof(AllSongs) / sizeof(AllSongs[0]);
    if (numSongs == 0) {
        printf("No songs in local storage.\n");
        return;
    }
    int randomIndex = rand() % numSongs;
    playSong(AllSongs[randomIndex]);
    system("clear");
    printf("******\n\n");
    sleep(3);
}

void playMostPlayedSong() {
    int maxPlayCount = -1;
    int mostPlayedSongIndex = -1;
    for (int i = 0; i < 7; i++) {
        if (SongPlayCounts[i] > maxPlayCount) {
            maxPlayCount = SongPlayCounts[i];
            mostPlayedSongIndex = i;
        }
    }
    if (mostPlayedSongIndex >= 0) {
        printf("Playing the most played song: %s\n", AllSongs[mostPlayedSongIndex]);
        playSong(AllSongs[mostPlayedSongIndex]);
    } else {
        printf("No songs have been played yet.\n");
    }
    system("clear");
    printf("******\n\n");
    sleep(3);
}

void searchAndPlay() {
    char songName[100];
    printf("Enter the name of the song you want to play: ");
    scanf("%s", songName);
    int songFound = 0;
    for (int i = 0; i < 7; i++) {
        if (strcmp(AllSongs[i], songName) == 0) {
            playSong(AllSongs[i]);
            songFound = 1;
            printf("Now playing: %s\n", AllSongs[i]);
            system("clear");
            printf("******\n\n");
            sleep(3);
            break;
        }
    }
    if (!songFound) {
        printf("Song not found: %s\n", songName);
    }
}

void printPlaylist(Node *head) {
  if (head == NULL) {
    printf("Playlist is empty.\n");
  } else {
      Node *current = head->next;
      while (current != head) {
        for (int i=0;i<7;i++) {
          if (strcmp(AllSongs[i], current->songName) == 0) {
            printf("%s\n",current->songName);
          }
        }
        current = current->next;
      }
      for (int i=0;i<7;i++) {
        if (strcmp(AllSongs[i], current->songName) == 0) {
          printf("%s\n",current->songName);
        }
      }
  }
}

void showMusicStats()
{
    char songName[100];
    printf("Enter the name of the song you want to show the statistics for: ");
    scanf("%s", songName);
    int songFound = 0;
    for (int i = 0; i < 7; i++) {
        if (strcmp(AllSongs[i], songName) == 0) {
            songFound = 1;
            printf("Music stats :\n%s\n", musicStats[i]);
            break;
        }
    }
    if (!songFound) {
        printf("Song not found: %s\n", songName);
    }
    sleep(10);
    system("clear");
}

int main() {
    int choice;
    Node *playlist = NULL;
    Queue *que = NULL;
    int runPgm = 1;
    srand(time(NULL));
    while (runPgm != -1) {
        print_menu();
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                playAllSongs();
                break;
            case 2: {
                char songName[100];
                printf("Enter the song name you want to add to your playlist: ");
                scanf("%s", songName);
                insertSong(&playlist, songName);
                printf("Song added to your playlist.\n");
                break;
            }
            case 3: {
                char songName[100];
                printf("Enter the song name you want to delete from your playlist: ");
                scanf("%s", songName);
                deleteSong(&playlist, songName);
                break;
            }
            case 4:
                if (playlist == NULL) {
                    printf("Playlist is empty\n");
                } else {
                  playPlaylist(playlist);
                }
                break;
            case 5:
                printPlaylist(playlist);
                break;
            case 6:
                que = createQ();
                playQ(que);
                break;
            case 7:
                playRandomSong();
                break;
            case 8 :
                playMostPlayedSong();
                break;
            case 9 :
                searchAndPlay();
                break;
            case 10:
                showMusicStats();
                break;
            default :
                printf("Wrong Entry, please enter a correct choice again\n");
                break;
        }
        printf("Enter -1 to exit or any other number to continue: ");
        scanf("%d", &runPgm);
        if (runPgm == -1) {
            freePlaylist(playlist);
            break;
        }
    }
    return 0;
}
