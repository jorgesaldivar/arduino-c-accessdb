//***************** vv DOCUMENTING AND HELPFUL STUFF vv ********************
	//C_Program_Register_SerialData_AccessDB.c
	
	//Compiled using Microsoft Visual Studio 11 found at
	//http://www.microsoft.com/visualstudio/en-us/products/2010-editions/express
	
	//Connection with the database with ODBC (x86, 64-bit) found at
	//http://dev.mysql.com/downloads/connector/odbc/
	//On Windows 7 64 bits
	
	
	//Created by Jorge Saldivar on May 13 2012.
	//using Luis Ricardo Salgado example for academic purposes

//***************** vv DOCUMENTING AND HELPFUL STUFF vv ********************

//Libraries used
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <sql.h>
#include <sqlext.h>
#include <time.h>



	//Variables used for the database
	HENV henv;	//Reference ("handle") to memory for environment variables / / ("Handle to environment")
	HDBC hdbc;	//Reference ("handle") to data connection (session) with the database ("Handle Conection to DataBase")
	RETCODE retcode;	//Return code for SQL operations ... actually 32-bit integer.
	HSTMT hstmt;		//Reference ("handle") to a statute ... actually is a STRING

	//It keeps all the query in this chain of chars
	char query[300];

	//Variables for Serial port interaction
	HANDLE serialPort;
	DCB protocol;

	//Chars to handle transfer of Serial Port
	char oneLetter;
	char multipleLetters[30];
	int readFromSerial,writedToSerial;
	
	//Variables for sync the serial data
	int sync=0; //Sync data
	int syncronizedForTwoPoints = 0;	//Sync for protocol used
	int counterForSync = 0; //Sincroniza los datos
	int addForId; //Variable para guardar el id del usuario

	//Variables for methods
	int checkTheExit,checkTheTime,dayOfWeek;

	//Variables that uses SQL to get data from tables
	char door;	//door number
	char name[51];	//array of 51 chars
	int hours,theDate,countDoors,validateTime;
	SDWORD bufferForSql,secondBufferForSql,fourthBufferForSql,fifthBufferForSql;
	SQLINTEGER thirdBufferForSql;
	SQL_TIMESTAMP_STRUCT StatusTime; //Get time from SQL
	
	//Funcion para checar la salida
	int checkExit(int addForId, int door){
			
			//REMOVE - it only prints data
			printf("\n\n");
			//REMOVE
			
			//If the count is more than cero, it means the employee is inside the building
			sprintf(query,"SELECT Count(*) AS Salida FROM LogAsistencia WHERE (((LogAsistencia.[IDGaffete])=%d) AND ((LogAsistencia.[Entrada]) Is Not Null) AND ((LogAsistencia.[Salida]) Is Null))", addForId);
			
				//REMOVE - it only prints data
				printf(query);
				printf("\n\n");
				//REMOVE
				
				// Allocate memory for the statement handle
				retcode = SQLAllocStmt (hdbc, &hstmt);
												
				// Prepare the SQL statement by assigning it to the statement handle
				retcode = SQLPrepare(hstmt, query ,SQL_NTS);
												
				// Execute the SQL statement handle
				retcode = SQLExecute(hstmt); // Execute statement with

				// Takes SQL numbers
				SQLBindCol(hstmt, 1, SQL_C_LONG, &hours, 0, &secondBufferForSql);
												
				// Get row of data from the result set defined above in the statement
				retcode = SQLFetch(hstmt);

			while (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) { 
				//Prints the exit hour
				printf("Salida es 1 Entrada es 0 \nEntrada/Salida: %d \n",hours);
				
				// Fetch next row from result set				
				retcode = SQLFetch (hstmt);
			}
			
			//If the count was more than cero, it means the employee tries to exit. Enter this function if true.
			if(hours != 0){

					//Protocol to send the message to open door
					multipleLetters[0] = '3';
					multipleLetters[1] = ':';
					multipleLetters[2] = door;
					multipleLetters[3] = '-';
					multipleLetters[4] = 's';
					multipleLetters[5] = '\n';
					WriteFile(serialPort,&multipleLetters,6,(LPDWORD)&writedToSerial,NULL); //Sends the message
					
					//REMOVE
					printf("\n\n");
					//REMOVE
					
					//Registers the user in the access log and puts a 3 in autorizacion. 3 means exit
					sprintf(query,"INSERT INTO LogAccesos (IDGaffete, Puerta,Hora,Autorizado) VALUES ('%d','%d',NOW(),'3');",addForId,door-48);
				    
						//REMOVE
						printf(query);
						printf("\n\n");
						//REMOVE
					
						// Allocate memory for the statement handle
						retcode = SQLAllocStmt (hdbc, &hstmt);
												
						// Prepare the SQL statement by assigning it to the statement handle
						retcode = SQLPrepare(hstmt, query ,SQL_NTS);
												
						// Execute the SQL statement handle
						retcode = SQLExecute(hstmt); // Execute statement with
					
					//REMOVE
					printf("\n\n");
					//REMOVE
					
					//Set the time when the employee exit the building
					sprintf(query,"UPDATE LogAsistencia SET Salida=NOW() WHERE LogAsistencia.IDGaffete = %d ;" ,addForId);
					
						//REMOVE
						printf(query);
						printf("\n\n");
						//REMOVE
					
						// Allocate memory for the statement handle
						retcode = SQLAllocStmt (hdbc, &hstmt);
												
						// Prepare the SQL statement by assigning it to the statement handle
						retcode = SQLPrepare(hstmt, query ,SQL_NTS);
												
						// Execute the SQL statement handle
						retcode = SQLExecute(hstmt); // Execute statement with

					//REMOVE
					printf("\n\n");
					//REMOVE
					
					//Set the hours work when the employee entered and get out of the building
					sprintf(query,"UPDATE LogAsistencia SET HorasTrabajadas=DateDiff('h',Entrada,NOW()) WHERE IDGaffete=%d;",addForId);
					
						//REMOVE
						printf(query);
						printf("\n\n");
						//REMOVE

						// Allocate memory for the statement handle
						retcode = SQLAllocStmt (hdbc, &hstmt);
												
						// Prepare the SQL statement by assigning it to the statement handle
						retcode = SQLPrepare(hstmt, query ,SQL_NTS);
												
						// Execute the SQL statement handle
						retcode = SQLExecute(hstmt); // Execute statement with

				return 1;
			}

			else {
				return 0;
			}

	}



	int checkDoor(int addForId, int door){
			
			//REMOVE
			printf("\n\n");
			//REMOVE
			
			//Check if the user have access to the door. If count is more than 0 it means it does have access
			sprintf(query,"SELECT Count(*) AS Puertas FROM Puertas WHERE IDGaffete=%d AND Puerta=%d",addForId,door-48);
			
				//REMOVE
				printf(query);
				printf("\n\n");
				//REMOVE
			
				// Allocate memory for the statement handle
				retcode = SQLAllocStmt (hdbc, &hstmt);
												
				// Prepare the SQL statement by assigning it to the statement handle
				retcode = SQLPrepare(hstmt, query ,SQL_NTS);
												
				// Execute the SQL statement handle
				retcode = SQLExecute(hstmt); // Execute statement with

				// Takes SQL numbers
				SQLBindCol(hstmt, 1, SQL_C_LONG, &countDoors, 0, &fourthBufferForSql);

				// Get row of data from the result set defined above in the statement
				retcode = SQLFetch(hstmt);

				//If count is more than cero, it means it does have access. Enter the function if true.
				if(countDoors != 0){
						return 1;
				
				}		

				else {
					return 0;
				}

	}


	void checkTime(int addForId,int door){

				//Day of week format 
				/*
				1 = Sunday
				2 = Monday
				3 = Tuesday
				4 = Wednesday
				5 = Thurday
				6 = Friday
				7 = Saturday
				*/
				
				//REMOVE
				printf("\n\n");
				//REMOVE

				//Returns the day of week
				sprintf(query,"SELECT Weekday (NOW());");

					//REMOVE
					printf(query);
					printf("\n\n");
					//REMOVE

					// Allocate memory for the statement handle
					retcode = SQLAllocStmt (hdbc, &hstmt);
											
					// Prepare the SQL statement by assigning it to the statement handle
					retcode = SQLPrepare(hstmt, query ,SQL_NTS);
											
					// Execute the SQL statement handle
					retcode = SQLExecute(hstmt); // Execute statement with

					// Takes SQL numbers
					SQLBindCol(hstmt, 1, SQL_C_LONG, &validateTime, 0, &fifthBufferForSql);
											
					// Get row of data from the result set defined above in the statement
					retcode = SQLFetch(hstmt);

					while (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) { 
						//Prints the exit hour
						printf("Domingo es igual a 1\nDia de la semana: %d \n",validateTime);
				
						// Fetch next row from result set				
						retcode = SQLFetch (hstmt);
					}

					//If its Monday - Friday
					if(validateTime >= 2 && validateTime <= 6){


							//REMOVE
							printf("\n\n");
							//REMOVE
				
							//Check if employee can enter between his access hours from Monday to friday
							sprintf(query,"SELECT Count(*) As Valido FROM Restricciones WHERE time()>=Lu_ViMin AND time()<=Lu_ViMax AND IDGaffete=%d;",addForId);
				
								//REMOVE
								printf(query);
								printf("\n\n");
								//REMOVE

								// Allocate memory for the statement handle
								retcode = SQLAllocStmt (hdbc, &hstmt);
											
								// Prepare the SQL statement by assigning it to the statement handle
								retcode = SQLPrepare(hstmt, query ,SQL_NTS);
											
								// Execute the SQL statement handle
								retcode = SQLExecute(hstmt); // Execute statement with

								// Takes SQL numbers
								SQLBindCol(hstmt, 1, SQL_C_LONG, &validateTime, 0, &fifthBufferForSql);
											
								// Get row of data from the result set defined above in the statement
								retcode = SQLFetch(hstmt);

								//Check if employee is on selected hours. Enter function
								if(validateTime != 0){

										
										//REMOVE
										printf("\n\n");
										//REMOVE
					
										//Insert into access log that the employee has access to the door. 1 means autorization is valid.
										sprintf(query,"INSERT INTO LogAccesos (IDGaffete, Puerta,Hora,Autorizado) VALUES ('%d','%d',NOW(),'1');",addForId,door-48);
				    
											//REMOVE
											printf(query);
											printf("\n\n");
											//REMOVE
					
											// Allocate memory for the statement handle
											retcode = SQLAllocStmt (hdbc, &hstmt);
												
											// Prepare the SQL statement by assigning it to the statement handle
											retcode = SQLPrepare(hstmt, query ,SQL_NTS);
												
											// Execute the SQL statement handle
											retcode = SQLExecute(hstmt); // Execute statement with
					


										//REMOVE
										printf("\n\n");
										//REMOVE
					
										//Insert the time the employee entered the building
										sprintf(query,"INSERT INTO LogAsistencia (IDGaffete, Entrada) VALUES ('%d',NOW());",addForId);
					
											//REMOVE
											printf(query);
											printf("\n\n");
											//REMOVE

											// Allocate memory for the statement handle
											retcode = SQLAllocStmt (hdbc, &hstmt);
												
											// Prepare the SQL statement by assigning it to the statement handle
											retcode = SQLPrepare(hstmt, query ,SQL_NTS);
												
											// Execute the SQL statement handle
											retcode = SQLExecute(hstmt); // Execute statement with
								}		

								else {
									
									//Protocol to send the message to close door
									multipleLetters[0] = '3';
									multipleLetters[1] = ':';
									multipleLetters[2] = door;
									multipleLetters[3] = '-';
									multipleLetters[4] = 'n';
									multipleLetters[5] = '\n';
									WriteFile(serialPort,&multipleLetters,6,(LPDWORD)&writedToSerial,NULL); //Sends the message

									//REMOVE
									printf("\n\n");
									//REMOVE
				
									//Register in the access log that the employee tried to get in, but was not autorized. 2 is for not autorized.
									sprintf(query,"INSERT INTO LogAccesos (IDGaffete, Puerta,Hora,Autorizado) VALUES ('%d','%d',NOW(),'2');",addForId,door-48);
				
										//REMOVE
										printf(query);
										printf("\n\n");
										//REMOVE

										// Allocate memory for the statement handle
										retcode = SQLAllocStmt (hdbc, &hstmt);
											
										// Prepare the SQL statement by assigning it to the statement handle
										retcode = SQLPrepare(hstmt, query ,SQL_NTS);
											
										// Execute the SQL statement handle
										retcode = SQLExecute(hstmt); // Execute statement with

								}

					}

					//If its Sunday
					else if (validateTime == 1){


							//REMOVE
							printf("\n\n");
							//REMOVE
				
							//Check if employee can enter between his access hours on Sunday
							sprintf(query,"SELECT Count(*) As Valido FROM Restricciones WHERE time()>=DomingoMin AND time()<=DomingoMax AND IDGaffete=%d;",addForId);
				
								//REMOVE
								printf(query);
								printf("\n\n");
								//REMOVE

								// Allocate memory for the statement handle
								retcode = SQLAllocStmt (hdbc, &hstmt);
											
								// Prepare the SQL statement by assigning it to the statement handle
								retcode = SQLPrepare(hstmt, query ,SQL_NTS);
											
								// Execute the SQL statement handle
								retcode = SQLExecute(hstmt); // Execute statement with

								// Takes SQL numbers
								SQLBindCol(hstmt, 1, SQL_C_LONG, &validateTime, 0, &fifthBufferForSql);
											
								// Get row of data from the result set defined above in the statement
								retcode = SQLFetch(hstmt);

								if(validateTime != 0){

										
										//REMOVE
										printf("\n\n");
										//REMOVE
					
										//Insert into access log that the employee has access to the door. 1 means autorization is valid.
										sprintf(query,"INSERT INTO LogAccesos (IDGaffete, Puerta,Hora,Autorizado) VALUES ('%d','%d',NOW(),'1');",addForId,door-48);
				    
										//REMOVE
										printf(query);
										printf("\n\n");
										//REMOVE
					
											// Allocate memory for the statement handle
											retcode = SQLAllocStmt (hdbc, &hstmt);
												
											// Prepare the SQL statement by assigning it to the statement handle
											retcode = SQLPrepare(hstmt, query ,SQL_NTS);
												
											// Execute the SQL statement handle
											retcode = SQLExecute(hstmt); // Execute statement with
					


											//REMOVE
											printf("\n\n");
											//REMOVE
					
										//Insert the time the employee entered the building
										sprintf(query,"INSERT INTO LogAsistencia (IDGaffete, Entrada) VALUES ('%d',NOW());",addForId);
						
											//REMOVE
											printf(query);
											printf("\n\n");
											//REMOVE

											// Allocate memory for the statement handle
											retcode = SQLAllocStmt (hdbc, &hstmt);
												
											// Prepare the SQL statement by assigning it to the statement handle
											retcode = SQLPrepare(hstmt, query ,SQL_NTS);
												
											// Execute the SQL statement handle
											retcode = SQLExecute(hstmt); // Execute statement with
								}		

								else {
									
									//Protocol to send the message to close door
									multipleLetters[0] = '3';
									multipleLetters[1] = ':';
									multipleLetters[2] = door;
									multipleLetters[3] = '-';
									multipleLetters[4] = 'n';
									multipleLetters[5] = '\n';
									WriteFile(serialPort,&multipleLetters,6,(LPDWORD)&writedToSerial,NULL); //Sends the message

									//REMOVE
									printf("\n\n");
									//REMOVE
				
									//Register in the access log that the employee tried to get in, but was not autorized. 2 is for not autorized.
									sprintf(query,"INSERT INTO LogAccesos (IDGaffete, Puerta,Hora,Autorizado) VALUES ('%d','%d',NOW(),'2');",addForId,door-48);
				
										//REMOVE
										printf(query);
										printf("\n\n");
										//REMOVE

										// Allocate memory for the statement handle
										retcode = SQLAllocStmt (hdbc, &hstmt);
											
										// Prepare the SQL statement by assigning it to the statement handle
										retcode = SQLPrepare(hstmt, query ,SQL_NTS);
											
										// Execute the SQL statement handle
										retcode = SQLExecute(hstmt); // Execute statement with

								}

					}

					//If its Saturday
					else if(validateTime == 7){


							//REMOVE
							printf("\n\n");
							//REMOVE
				
							//Check if employee can enter between his access hours on Saturday
							sprintf(query,"SELECT Count(*) As Valido FROM Restricciones WHERE time()>=SabadoMin AND time()<=SabadoMax AND IDGaffete=%d;",addForId);
				
								//REMOVE
								printf(query);
								printf("\n\n");
								//REMOVE

								// Allocate memory for the statement handle
								retcode = SQLAllocStmt (hdbc, &hstmt);
											
								// Prepare the SQL statement by assigning it to the statement handle
								retcode = SQLPrepare(hstmt, query ,SQL_NTS);
											
								// Execute the SQL statement handle
								retcode = SQLExecute(hstmt); // Execute statement with

								// Takes SQL numbers
								SQLBindCol(hstmt, 1, SQL_C_LONG, &validateTime, 0, &fifthBufferForSql);
											
								// Get row of data from the result set defined above in the statement
								retcode = SQLFetch(hstmt);

								if(validateTime != 0){

										
										//REMOVE
										printf("\n\n");
										//REMOVE
					
										//Insert into access log that the employee has access to the door. 1 means autorization is valid.
										sprintf(query,"INSERT INTO LogAccesos (IDGaffete, Puerta,Hora,Autorizado) VALUES ('%d','%d',NOW(),'1');",addForId,door-48);
				    
											//REMOVE
											printf(query);
											printf("\n\n");
											//REMOVE
						
											// Allocate memory for the statement handle
											retcode = SQLAllocStmt (hdbc, &hstmt);
												
											// Prepare the SQL statement by assigning it to the statement handle
											retcode = SQLPrepare(hstmt, query ,SQL_NTS);
												
											// Execute the SQL statement handle
											retcode = SQLExecute(hstmt); // Execute statement with
					


										//REMOVE
										printf("\n\n");
										//REMOVE
					
										//Insert the time the employee entered the building
										sprintf(query,"INSERT INTO LogAsistencia (IDGaffete, Entrada) VALUES ('%d',NOW());",addForId);
					
											//REMOVE
											printf(query);
											printf("\n\n");
											//REMOVE

											// Allocate memory for the statement handle
											retcode = SQLAllocStmt (hdbc, &hstmt);
												
											// Prepare the SQL statement by assigning it to the statement handle
											retcode = SQLPrepare(hstmt, query ,SQL_NTS);
												
											// Execute the SQL statement handle
											retcode = SQLExecute(hstmt); // Execute statement with
								}		

								else {
									
									//Protocol to send the message to close door
									multipleLetters[0] = '3';
									multipleLetters[1] = ':';
									multipleLetters[2] = door;
									multipleLetters[3] = '-';
									multipleLetters[4] = 'n';
									multipleLetters[5] = '\n';
									WriteFile(serialPort,&multipleLetters,6,(LPDWORD)&writedToSerial,NULL); //Sends the message

									//REMOVE
									printf("\n\n");
									//REMOVE
				
									//Register in the access log that the employee tried to get in, but was not autorized. 2 is for not autorized.
									sprintf(query,"INSERT INTO LogAccesos (IDGaffete, Puerta,Hora,Autorizado) VALUES ('%d','%d',NOW(),'2');",addForId,door-48);
				
									//REMOVE
									printf(query);
									printf("\n\n");
									//REMOVE

										// Allocate memory for the statement handle
										retcode = SQLAllocStmt (hdbc, &hstmt);
											
										// Prepare the SQL statement by assigning it to the statement handle
										retcode = SQLPrepare(hstmt, query ,SQL_NTS);
											
										// Execute the SQL statement handle
										retcode = SQLExecute(hstmt); // Execute statement with

								}

					}

				
	}


	void main() {
		
		serialPort = CreateFile("COM3",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL); //opens the communication port for READ and WRITE
		
		//If it can conect to the serial port, it enter t
		if (serialPort != INVALID_HANDLE_VALUE) {

			//Initialize the serial protocol
			GetCommState(serialPort,&protocol);
			protocol.BaudRate = CBR_9600;
			protocol.fBinary = TRUE;
			protocol.fParity = FALSE;
			protocol.ByteSize = 8;
			protocol.Parity = NOPARITY;
			protocol.StopBits = ONESTOPBIT;
			SetCommState(serialPort,&protocol);
			
			
			retcode = SQLAllocEnv(&henv); //Reference ("handle") memory space environment variables
			if (retcode == SQL_SUCCESS) {
				retcode = SQLAllocConnect(henv, &hdbc); //Reference to session data
				if (retcode == SQL_SUCCESS) {
					retcode = SQLConnect(hdbc, "dbPieci", SQL_NTS, NULL, 0, NULL, 0); //Connect to database with ODBC
					if (retcode == SQL_SUCCESS) {
						printf("Se conecto exitosamente con la base de datos\n");
						retcode = SQLAllocStmt(hdbc, &hstmt); //Reference to the SQL statement
						if (retcode == SQL_SUCCESS) {
							
							//The program is always running
							while(0 == 0) {
			
								ReadFile(serialPort,&oneLetter,1,(LPDWORD)&readFromSerial,NULL); //Serial data read and stored in the variable char: letter
			
								//If it read something it enters the function
								if (readFromSerial!=0) {
										
										//Protocol to receive data by initializing the number 3
										if (oneLetter == '3') {
											syncronizedForTwoPoints = 1;
											printf("%c",oneLetter);				//Prints number in screen
										}

											//Is protocol "3:" is read it enters this function
											if (sync) {

												if(counterForSync == 0 ){
													printf("%c",oneLetter);	//Prints in screen
													door = oneLetter;		//Saves door number
												}

												else if(counterForSync == 1 ){
													printf("%c",oneLetter);	//Prints in screen
												}

												else if(counterForSync == 2 ){
													addForId = oneLetter * 256;	//Saves user ID part of it
												}
												
												//Not do anything because of protocol char in 3 position doesnt have anything useful
												else if(counterForSync == 3 ){	
												}

												else  if(counterForSync == 4 ){

													addForId += oneLetter; 	//Saves user ID correctly
													printf("%d\n",addForId);	//Prints user ID
													
													//CheckExit return 1 if its an exit
													checkTheExit = checkExit(addForId,door);
													
													//If its not an exit, validates door.
													if (checkTheExit == 0){
														checkTheTime = checkDoor(addForId,door);

														//If the employee is registered in the door, then check the time to know if the employee can enter
														if(checkTheTime == 1){
															checkTime(addForId,door);
														}
														
														else {
															
															//Protocol to send the message to close door
															multipleLetters[0] = '3';
															multipleLetters[1] = ':';
															multipleLetters[2] = door;
															multipleLetters[3] = '-';
															multipleLetters[4] = 'n';
															multipleLetters[5] = '\n';
															WriteFile(serialPort,&multipleLetters,6,(LPDWORD)&writedToSerial,NULL); //Sends the message

														}
														
													}


											
													/*
													//Get SQL Data con given Format
													//Get string
													SQLBindCol(hstmt, 1, SQL_C_CHAR, name, 51, &bufferForSql);
													//Get Numbers
													SQLBindCol(hstmt, 2, SQL_C_LONG, &hours, 0, &secondBufferForSql);
													//Get Date
													SQLBindCol(hstmt, 3, SQL_C_TIMESTAMP, &StatusTime, 0, &thirdBufferForSql); 								
													//Date is dislayed this way:
													printf("Los dias : %d-%d-%d %d:%d:%d\n",StatusTime.year,StatusTime.day,StatusTime.month,StatusTime.hour,StatusTime.minute,StatusTime.second);
													*/

												}	
												
												//Add Sync Counter
												counterForSync++;

												//Resets the syncF
												if(counterForSync == 5 ){
													sync = 0;
													counterForSync = 0;
												}
												
												
											}

										//If the letter 3 was previosly read, it enters this function
										if(oneLetter == ':' && syncronizedForTwoPoints == 1){
											printf("%c",oneLetter);				//Prints number in screen
											syncronizedForTwoPoints = 0;
											sync = 1;							//Syncs
										}


								}
							}
						}
					}
				}
			}
			// Free the allocated statement handle
			SQLFreeStmt (hstmt, SQL_DROP);
			CloseHandle(serialPort);
		}
	}