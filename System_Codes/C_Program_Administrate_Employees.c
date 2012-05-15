//***************** vv DOCUMENTING AND HELPFUL STUFF vv ********************
	//C_Program_Administrate_Employees.c
	
	//Compiled using Microsoft Visual Studio 11 found at
	//http://www.microsoft.com/visualstudio/en-us/products/2010-editions/express
	
	//Connection with the database with ODBC (x86, 64-bit) found at
	//http://dev.mysql.com/downloads/connector/odbc/
	//On Windows 7 64 bits
	
	
	//Created by Jorge Saldivar on May 13 2012.

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
	HSTMT hstmt2;		//Reference ("handle") to a statute ... actually is a STRING

	//Chars to handle transfer of Serial Port
	char option;
	char query[300];
	char query2[300];
	char optionUser[5];

	//Read string from keyboard 										
	char year[5]; 
	char month[3]; 
	char day[3];
	char optionKeyboard[5];

	//For payroll
	char countablePeriod[5];
	char initialYear[5]; 
	char initialMonth[3]; 
	char initialDay[3];

	int workedHoursIncome,extraHoursIncome, netIncome;
	float ISPT, IMSS, grossIncome;

	char finalYear[5]; 
	char finalMonth[3]; 
	char finalDay[3];

	int sumOfWorkedHoursPayroll,sumOfExtraHoursPayroll;

	//For query
	int workedHours;
	int extraHours;
	int wage,extraWage;
	char completeDate[14];
	char initialDate[14];
	char finalDate[14];

	//Not exit program variable
	int cycle = 1;

	//Variables for methods
	int checkTheExit,checkTheTime,dayOfWeek;

	//Variables that uses SQL to get data from tables
	char door;	//door number
	char name[51];	//array of 51 chars
	int hours,sumOfHours,payroll,badge;
	SDWORD bufferForSql,secondBufferForSql,fourthBufferForSql,fifthBufferForSql;
	//SQLINTEGER thirdBufferForSql;
	//SQL_TIMESTAMP_STRUCT StatusTime; //Get time from SQL

	//Method for calculating the daily asistance and insert it on the database
	void dailyAsistance(){
										
			printf( "(Ejemplo 2012) Escribe ano: " );

			/* notice stdin being passed in */
			fgets ( year, 5, stdin );           

		    getchar();						                              

					printf( "(Ejemplo 05) Escribe mes: " );

					/* notice stdin being passed in */
					fgets ( month, 3, stdin );           

					getchar();                            

								printf( "(Ejemplo 03) Escribe dia: " );

								/* notice stdin being passed in */		
								fgets ( day, 3, stdin );           

								getchar();




					//Query for getting the id of the employee the sum of worked hours and the employee badge
					sprintf(query, "SELECT E.IDGaffete, E.NominaEmpleado, Sum(LA.HorasTrabajadas) AS SumOfHorasTrabajadas FROM LogAsistencia AS LA, Empleado AS E WHERE (((E.IDGaffete)=[LA].[IDGaffete]) AND ((DatePart('yyyy',[Salida]))=%s) AND ((DatePart('m',[Salida]))=%s) AND ((DatePart('d',[Salida]))=%s)) GROUP BY E.IDGaffete, E.NominaEmpleado;", year,month,day);
					
					//REMOVE
					printf(query);
					//REMOVE

							// Allocate memory for the statement handle
							retcode = SQLAllocStmt (hdbc, &hstmt);
											
							// Prepare the SQL statement by assigning it to the statement handle
							retcode = SQLPrepare(hstmt, query ,SQL_NTS);
											
							// Execute the SQL statement handle
							retcode = SQLExecute(hstmt); // Execute statement with

							// Takes SQL numbers
							SQLBindCol(hstmt, 1, SQL_C_LONG, &badge, 0, &fifthBufferForSql);
							SQLBindCol(hstmt, 2, SQL_C_LONG, &payroll, 0, &fifthBufferForSql);
							SQLBindCol(hstmt, 3, SQL_C_LONG, &sumOfHours, 0, &fifthBufferForSql);
											
							// Get row of data from the result set defined above in the statement
							retcode = SQLFetch(hstmt);
														
							while (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) { 
								
								//Calculate worked hours and extra hours from the total of hours worked in one day
								if(sumOfHours >= 8){
									workedHours = 8;
									extraHours = sumOfHours - workedHours;
								}
															
								else{
									extraHours = 0;
									workedHours = sumOfHours;
								}

								//Put the day month and year asked to the used previously in one string
								sprintf(completeDate,"%s/%s/%s",day,month,year);

								//Query for inserting the daily asistance
								sprintf(query2, "INSERT INTO AsistenciaDiaria (Fecha, HorasDeTrabajo, HorasExtra, NominaEmpleado) VALUES ('%s', %d, %d, %d)",completeDate,workedHours,extraHours,payroll);
															
								//REMOVE
								printf(query2);
								//REMOVE

										// Allocate memory for the statement handle
										retcode = SQLAllocStmt (hdbc, &hstmt2);
											
										// Prepare the SQL statement by assigning it to the statement handle
										retcode = SQLPrepare(hstmt2, query2 ,SQL_NTS);
											
										// Execute the SQL statement handle
										retcode = SQLExecute(hstmt2); // Execute statement with
											
										// Get row of data from the result set defined above in the statement
										retcode = SQLFetch(hstmt2);

								// Fetch next row from result set				
								retcode = SQLFetch (hstmt);
							}
							printf("\nSe ejecuto exitosamente!\n\n"); //Print that everything went successfull
	}

	//method for generating payroll
	void generatePayroll(){

				printf( "(Ejemplo 8) Periodo contable: " );

					/* notice stdin being passed in */
					fgets ( countablePeriod, 3, stdin );           

					getchar();	



				printf( "(Ejemplo 2012) Escribe ano inicio: " );

					/* notice stdin being passed in */
					fgets (initialYear, 5, stdin );           

					 getchar();	


							printf( "(Ejemplo 05) Escribe mes inicio: " );

								/* notice stdin being passed in */
								fgets ( initialMonth, 3, stdin );           

								getchar();                            

										printf( "(Ejemplo 03) Escribe dia inicio: " );

															
											fgets ( initialDay, 3, stdin );           

											getchar();

					printf( "(Ejemplo 2012) Escribe ano fin: " );

						/* notice stdin being passed in */
						fgets ( finalYear, 5, stdin );           

						getchar();						                              

									printf( "(Ejemplo 05) Escribe mes fin: " );

										/* notice stdin being passed in */
										fgets ( finalMonth, 3, stdin );           

										getchar();                            

												printf( "(Ejemplo 03) Escribe dia fin: " );

													/* notice stdin being passed in */		
													fgets ( finalDay, 3, stdin );           

													getchar();

							//Query for getting the sum of hours worked, the sum of extra hours, the employee badge, salary and extra salary
							sprintf(query, "SELECT SUM(HorasDeTrabajo) AS HorasDeTrabajoAcum, SUM(HorasExtra) AS HorasExtraAcum, AsistenciaDiaria.NominaEmpleado, Salario, SalarioExtra FROM AsistenciaDiaria, Empleado WHERE Empleado.NominaEmpleado = AsistenciaDiaria.NominaEmpleado AND ((DatePart('yyyy',[Fecha])) >= %s) AND ((DatePart('yyyy',[Fecha])) <= %s) AND ((DatePart('m',[Fecha])) >=%s) AND ((DatePart('m',[Fecha])) <=%s) AND ((DatePart('d',[Fecha])) >=%s) AND ((DatePart('d',[Fecha])) <=%s) GROUP BY AsistenciaDiaria.NominaEmpleado, Salario, SalarioExtra", initialYear,finalYear,initialMonth,finalMonth,initialDay,finalDay);
							
							//REMOVE
							printf(query);		
							//REMOVE

								// Allocate memory for the statement handle
								retcode = SQLAllocStmt (hdbc, &hstmt);
											
								// Prepare the SQL statement by assigning it to the statement handle
								retcode = SQLPrepare(hstmt, query ,SQL_NTS);
											
								// Execute the SQL statement handle
								retcode = SQLExecute(hstmt); // Execute statement with
								
								// Takes SQL numbers
								SQLBindCol(hstmt, 1, SQL_C_LONG, &sumOfWorkedHoursPayroll, 0, &fifthBufferForSql);
								SQLBindCol(hstmt, 2, SQL_C_LONG, &sumOfExtraHoursPayroll, 0, &fifthBufferForSql);
								SQLBindCol(hstmt, 3, SQL_C_LONG, &badge, 0, &fifthBufferForSql);
								SQLBindCol(hstmt, 4, SQL_C_LONG, &wage, 0, &fifthBufferForSql);
								SQLBindCol(hstmt, 5, SQL_C_LONG, &extraWage, 0, &fifthBufferForSql);
											
								// Get row of data from the result set defined above in the statement
								retcode = SQLFetch(hstmt);
													
							while (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) { 
															
								//Put the day month and year asked to the used previously in one string
								sprintf(initialDate,"'%s/%s/%s'",initialMonth,initialDay,initialYear);
								sprintf(finalDate,"'%s/%s/%s'",finalMonth,finalDay,finalYear);
								
								//Calculates the worked income, extra hours income, netincome, ISPT, IMSS and grossIncome
								workedHoursIncome = wage * sumOfWorkedHoursPayroll; 
								extraHoursIncome = extraWage * sumOfExtraHoursPayroll; 
								netIncome = workedHoursIncome + extraHoursIncome;
								ISPT =  (float) netIncome * .105;
								IMSS =  (float) netIncome * .0282;
								grossIncome = (float) netIncome - ISPT - IMSS;
								
								//Insert the data to the badge table
								sprintf(query2, "INSERT INTO Nomina (NominaEmpleado, PeriodoContable, InicioPeriodoContable, FinPeriodoContable, IngresoHorasDeTrabajo, IngresoHorasExtra, IngresoNeto, ISPT, IMSS, PagoTotal) VALUES (%d,%s,%s,%s,%d,%d,%d,%.2f,%.2f,%.2f)", badge, countablePeriod, initialDate, finalDate, workedHoursIncome, extraHoursIncome, netIncome, ISPT, IMSS, grossIncome);
								
								//REMOVE
								printf(query2);
								//REMOVE

										// Allocate memory for the statement handle
										retcode = SQLAllocStmt (hdbc, &hstmt2);
											
										// Prepare the SQL statement by assigning it to the statement handle
										retcode = SQLPrepare(hstmt2, query2 ,SQL_NTS);
											
										// Execute the SQL statement handle
										retcode = SQLExecute(hstmt2); // Execute statement with
											
										// Get row of data from the result set defined above in the statement
										retcode = SQLFetch(hstmt2);

								// Fetch next row from result set				
								retcode = SQLFetch (hstmt);
								
							}
							printf("\nSe ejecuto exitosamente!\n\n"); //Prints that everything went successfull


	}

	
	void main() {
			
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
								while(cycle == 1) {
									
										printf( "(Asistencia diaria = 1)\n(Genera Nomina = 2)\nElige opcion: " );

										/* notice stdin being passed in */
										fgets ( optionKeyboard, 2, stdin );           

										getchar();		

											//Put the char read in an array
											sprintf(optionUser,"%s",optionKeyboard);

											//If it reads a 1 it enters this funcion
											if(optionUser[0] == 49){
												//Calculate daily asistace from a given date
												dailyAsistance();
											}
											
											//If it reads a 2 it enters this funcion
											else if (optionUser[0] == 50){
												//Generate payroll of every employee from a given date
												generatePayroll();
											}

											else {
												//Type something again
												printf("\nEhhh?\n");
											}
								}

						}
					}
				}
			}
	}