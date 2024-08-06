import mysql.connector
import os
from dotenv import load_dotenv
load_dotenv()

db = {
    'host': os.environ.get('DB_HOST'),
    'user': os.environ.get('DB_USER'),
    'password': os.environ.get('DB_PASSWORD'),
    'database': os.environ.get('DB_DATABASE')
}

def query(query):
    connection = None
    cursor = None
    try:
        connection = mysql.connector.connect(**db)
        cursor = connection.cursor()
        cursor.execute(query)
        results = cursor.fetchall()
        return results 
    except mysql.connector.Error as e:
        return None
    finally:
        if cursor:
            cursor.close()
        if connection and connection.is_connected():
            connection.close()