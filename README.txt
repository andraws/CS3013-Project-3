
Project 3: threads and Syncronization
Library: pthread
    - Mutexes:
        pthread_mutex_lock & pthread_mutex_unlocked
    - Semaphores (condition)
        sem_wait, sem_post, sem_init
Goal: Create threads for each of our actors and debug synchronously.
    - Each thread uses randomness for scheduling (for testing use same seed)
Command Line Args:
    1. Number of costuming teams (2 min, 4 max)
    2. Number of pirates (10-50)
    3. Number of ninjas (10-50)
    4. Average time pirates spend in costume shop
    5. Average time ninjas spend in costume shop
    6. Average time pirates spend before revisiting costume shop
    7. Average time ninjas spend before revisiting costume shop
Problem:
    Any any given day, 10-50 pirates or ninjas enter our store
    Each staff can take care of appropriate amount of pirates or ninjas but never both
    Pirates and ninjas should identify themselves and which team it is using when entering or leaving
    Arrivals will be at random, and the amount of time that any individual takes to be outfited is random
    At end of custome session, there is a 25% chance they will come back
    Department keeps stats for billing, 1 gold, if wait is over 30 mins prior to entering, it is free
    Each Pirate and ninja is represented as a unique thread
    Each execution represents the operation of costume shop over a single day
    Code that creates the requisite number of ninjas and pirate threads and use either semaphores or locks to synchronize costume department
    Costume prep using sleep() with val from random distribution
Costume Department Usage Stats for Billing
    Bill for each Pirates and Ninjas
        -Number of visits
        -Amount of time of each visit
        -Wait times
        -Total gold owed
    Expenses & Profits
        -Each time costs 5 gold per day
        -Amount of time each team was busy
        -Amount of time each team was free
        -average queue length
        -Gross revenue (amount of gold)
        -Gold-per-visit
        -Profit
