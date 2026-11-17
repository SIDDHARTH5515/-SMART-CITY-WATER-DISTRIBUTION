#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_ZONES 3
#define RESERVOIR_CAPACITY 10000.0  
#define LOW_LEVEL_THRESHOLD 2000.0     


typedef struct {
    int zone_id;
    char name[30];
    float demand_liters;
    float inlet_flow;  
    float outlet_flow;  
    bool valve_open;
    bool leak_detected;
} CityZone;


typedef struct {
    float current_level;
    float ph_level;
    float turbidity_ntu;
    bool main_pump_on;
} MainReservoir;


void checkWaterQuality(MainReservoir *res);
void manageReservoirPump(MainReservoir *res);
void distributeWater(MainReservoir *res, CityZone zones[], int count);
void detectLeaks(CityZone zones[], int count);
void printSystemReport(MainReservoir res, CityZone zones[], int count);

int main() {
   
    MainReservoir reservoir = {
        .current_level = 6500.0,
        .ph_level = 7.2,
        .turbidity_ntu = 1.8,
        .main_pump_on = false
    };

    
    CityZone zones[MAX_ZONES] = {
        {1, "Residential_North", 1800.0, 0.0, 0.0, true, false},
        {2, "Commercial_Center", 2500.0, 0.0, 0.0, true, false},
        {3, "Industrial_South",  2000.0, 0.0, 0.0, true, false}
    };

    int choice;
    while (1) {
        printf("\n=== SMART CITY WATER DISTRIBUTION CONTROL ===\n");
        printf("1. Run Distribution & Monitoring Cycle\n");
        printf("2. Simulate Pipe Leak in Zone 2\n");
        printf("3. Simulate Water Contamination (High Turbidity)\n");
        printf("4. Refill Main Reservoir\n");
        printf("5. View System Status Report\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1:
                checkWaterQuality(&reservoir);
                manageReservoirPump(&reservoir);
                distributeWater(&reservoir, zones, MAX_ZONES);
                detectLeaks(zones, MAX_ZONES);
                printSystemReport(reservoir, zones, MAX_ZONES);
                break;

            case 2:
                printf("\n[SIMULATION] Pipe burst induced in %s!\n", zones[1].name);
                
                zones[1].inlet_flow = 2500.0;
                zones[1].outlet_flow = 1700.0; 
                detectLeaks(zones, MAX_ZONES);
                break;

            case 3:
                printf("\n[SIMULATION] Contamination detected in Main Reservoir!\n");
                reservoir.turbidity_ntu = 8.5; 
                checkWaterQuality(&reservoir);
                distributeWater(&reservoir, zones, MAX_ZONES);
                break;

            case 4:
                reservoir.current_level = RESERVOIR_CAPACITY;
                reservoir.turbidity_ntu = 1.5;
                reservoir.ph_level = 7.1;
                printf("\n[INFO] Reservoir refilled to %.1f L and water treated.\n", reservoir.current_level);
                break;

            case 5:
                printSystemReport(reservoir, zones, MAX_ZONES);
                break;

            case 6:
                printf("Shutting down Smart Water Control System.\n");
                return 0;

            default:
                printf("Invalid option. Try again.\n");
        }
    }
    return 0;
}


void checkWaterQuality(MainReservoir *res) {
    if (res->ph_level < 6.5 || res->ph_level > 8.5 || res->turbidity_ntu > 5.0) {
        printf("[ALERT] Unsafe Water Quality (pH: %.1f, Turbidity: %.1f NTU)! Supply Halted.\n",
               res->ph_level, res->turbidity_ntu);
    } else {
        printf("[OK] Water Quality Optimal (pH: %.1f, Turbidity: %.1f NTU).\n",
               res->ph_level, res->turbidity_ntu);
    }
}


void manageReservoirPump(MainReservoir *res) {
    if (res->current_level < LOW_LEVEL_THRESHOLD) {
        res->main_pump_on = true;
        printf("[PUMP] Reservoir low (%.1f L). Main intake pump ACTIVATED.\n", res->current_level);
        res->current_level += 3000.0; 
    } else {
        res->main_pump_on = false;
    }
}


void distributeWater(MainReservoir *res, CityZone zones[], int count) {
    bool quality_safe = (res->ph_level >= 6.5 && res->ph_level <= 8.5 && res->turbidity_ntu <= 5.0);

    for (int i = 0; i < count; i++) {
        if (!quality_safe || zones[i].leak_detected) {
            zones[i].valve_open = false;
            zones[i].inlet_flow = 0.0;
            zones[i].outlet_flow = 0.0;
            continue;
        }

        if (res->current_level >= zones[i].demand_liters) {
            zones[i].valve_open = true;
            zones[i].inlet_flow = zones[i].demand_liters;
            zones[i].outlet_flow = zones[i].demand_liters * 0.98; 
            res->current_level -= zones[i].demand_liters;
        } else {
            printf("[WARNING] Insufficient water for %s. Closing valve.\n", zones[i].name);
            zones[i].valve_open = false;
            zones[i].inlet_flow = 0.0;
            zones[i].outlet_flow = 0.0;
        }
    }
}


void detectLeaks(CityZone zones[], int count) {
    for (int i = 0; i < count; i++) {
        if (zones[i].inlet_flow > 0) {
            float loss_ratio = (zones[i].inlet_flow - zones[i].outlet_flow) / zones[i].inlet_flow;
            if (loss_ratio > LEAK_LOSS_THRESHOLD) {
                zones[i].leak_detected = true;
                zones[i].valve_open = false; 
                printf("[CRITICAL] Leak detected in %s (Loss: %.1f%%)! Solenoid valve CLOSED.\n",
                       zones[i].name, loss_ratio * 100);
            }
        }
    }
}


void printSystemReport(MainReservoir res, CityZone zones[], int count) {
    printf("\n--- TELEMETRY DASHBOARD ---\n");
    printf("Reservoir Level : %.1f / %.1f L | Intake Pump: %s\n",
           res.current_level, RESERVOIR_CAPACITY, res.main_pump_on ? "ON" : "OFF");
    printf("Water Quality   : pH %.1f | Turbidity %.1f NTU\n\n", res.ph_level, res.turbidity_ntu);

    printf("%-20s | %-10s | %-10s | %-10s | %-8s\n", "Zone", "Demand(L)", "Inlet(L)", "Outlet(L)", "Valve");
    printf("---------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-20s | %-10.1f | %-10.1f | %-10.1f | %-8s\n",
               zones[i].name,
               zones[i].demand_liters,
               zones[i].inlet_flow,
               zones[i].outlet_flow,
               zones[i].valve_open ? "OPEN" : "CLOSED");
    }
}
