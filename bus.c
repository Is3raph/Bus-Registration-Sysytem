#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//structure for bus
struct bus{
    int id;//id--busno
    char name[50];//driver's name
    int totalseats;
    int availableseats;
};

//structure for passenger
struct passenger{
    char name[50];
    int id;
    int seatno;
};

//function to add a bus 
void addbus(){
    struct bus bus;
    FILE *fp;
    fp=fopen("buses.bin","ab");
    if(fp==NULL){
        printf("File cannot be opened.\n");
        exit(1);
    }

    printf("Enter Bus no: ");
    scanf("%d",&bus.id);
    printf("Enter driver's name: ");
    scanf("%s",bus.name);
    printf("Enter total no of seats: ");
    scanf("%d",&bus.totalseats);
    bus.availableseats=bus.totalseats;//equate

    fwrite(&bus, sizeof(struct bus), 1, fp);
    fclose(fp);

    printf("...Bus added sucessfully\n");
}

//function to list the available buses
void listbuses(){
    struct bus bus;
    FILE *fp;
    fp=fopen("buses.bin","rb");
    if (fp==NULL) {
        printf("...No bus found\n");
        exit(0);
    }

    printf("\nList of Buses\n");
    while(fread(&bus, sizeof(struct bus), 1, fp)){
        printf("busno: %d, Driver: %s, Available Seats: %d/%d\n", bus.id, bus.name, bus.availableseats, bus.totalseats);
    }
    fclose(fp);
}

//function to book a seat
void bookseat(){
    struct passenger p;
    struct bus bus;
    FILE *busfile,*passengerfile;
    busfile=fopen("buses.bin","rb+");//yo
    passengerfile=fopen("passengers.bin","ab");
    if(busfile==NULL || passengerfile==NULL){
        printf("Error in opening the file!\n");
        exit(1);
    }

    printf("Enter your name: ");
    scanf("%s", p.name);
    printf("Enter Bus no: ");
    scanf("%d", &p.id);

    int found=0;
    while(fread(&bus, sizeof(struct bus), 1, busfile)) {
        if(bus.id==p.id && bus.availableseats>0){
            p.seatno=bus.totalseats-bus.availableseats+1;//p.seatno=passenger seat number
            bus.availableseats--;
            fseek(busfile,-sizeof(struct bus), 1);//(file,offset,position)
            fwrite(&bus, sizeof(struct bus), 1, busfile);
            fwrite(&p, sizeof(struct passenger), 1, passengerfile);
            found=1;
            printf("...Seat booked successfully! Your seat number is %d.\n", p.seatno);
            break;
        }
    }

    if(!found){//!found=!0=1
        printf("Bus not found or no available seats.\n");
    }

    fclose(busfile);
    fclose(passengerfile);
}

void cancelbooking() {
    struct passenger p;
    struct bus bus;
    FILE *busfile = fopen("buses.bin", "rb+");
    FILE *passfile = fopen("passengers.bin", "rb");
    FILE *tempfile = fopen("temp.bin", "wb");

    if (busfile==NULL || passfile==NULL || tempfile==NULL) {
        printf("Error in opening the file!\n");
        exit(1);
    }

    char name[50];
    int id;
    int found = 0;
    printf("Enter your name: ");
    scanf("%s", name);
    printf("Enter Bus no: ");
    scanf("%d", &id);

    struct passenger temppassengers[100]; // Temporary array to store remaining passengers
    int count = 0;

    // Read all passengers and remove the canceled one
    while (fread(&p, sizeof(struct passenger), 1, passfile)) {
        if (strcmp(p.name, name) == 0 && p.id == id) {
            // Update available seats in the bus
            while (fread(&bus, sizeof(struct bus), 1, busfile)) {
                if (bus.id == id) {
                    bus.availableseats++;
                    fseek(busfile, -sizeof(struct bus), 1);
                    fwrite(&bus, sizeof(struct bus), 1, busfile);
                    found = 1;
                    printf("...Booking cancelled successfully!\n");
                    break;
                }
            }
        } else {
            // Store remaining passengers in the array
            temppassengers[count++] = p;
        }
    }

    // Reassign seat numbers for the same bus
    for (int i = 0, seatno = 1; i < count; i++) {
        if (temppassengers[i].id == id) {
            temppassengers[i].seatno = seatno++;
        }
        fwrite(&temppassengers[i], sizeof(struct passenger), 1, tempfile);
    }

    if (!found) {
        printf("No matching booking found.\n");
    }

    fclose(busfile);
    fclose(passfile);
    fclose(tempfile);
    remove("passengers.bin");
    rename("temp.bin", "passengers.bin");
}

//function to view passengers
void viewpassengers(){
    struct passenger p;
    FILE *fp;
    fp=fopen("passengers.bin","rb");
    if(fp==NULL){
        printf("No passenger records found!\n");
        exit(0);
    }

    printf("\nList of Passengers:\n");
    while(fread(&p, sizeof(struct passenger), 1, fp)){
        printf("Name: %s, Bus no: %d, Seat no: %d\n", p.name, p.id, p.seatno);
    }
    fclose(fp);
}

int main() {
    int choice;
    
    do {
        printf("\nBus Registration System\n");
        printf("1. Add Bus\n");
        printf("2. List Buses\n");
        printf("3. Book a Seat\n");
        printf("4. Cancel Booking\n");
        printf("5. View Passengers\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                addbus();//call function addbus
                break;
            case 2:
                listbuses();
                break;
            case 3:
                bookseat();
                break;
            case 4:
                cancelbooking();
                break;
            case 5:
                viewpassengers();
                break;
            case 6:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid Entry!\n");
        }
    } while (choice != 6);
    
    return 0;
}