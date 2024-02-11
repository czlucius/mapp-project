import './App.css';
import 'chartjs-adapter-moment';
import {
    Chart as ChartJS,
    LineElement,
    TimeScale,
    Title,
    Tooltip,
    Legend
} from "chart.js";
import { Line } from 'react-chartjs-2';
import { DynamoDBClient, QueryCommand } from "@aws-sdk/client-dynamodb";
import * as keys from "./keys.json";
import { useState, useEffect } from 'react';

ChartJS.register(
    LineElement,
    TimeScale,
    Title,
    Tooltip,
    Legend
);

interface ValueTime {
    value: number;
    time: number;
}

const Source = ({ voltage, current, type }: {
    voltage: ValueTime[],
    current: ValueTime[],
    type: string
}) => {
    const vT = voltage.map(elem => elem.time);
    const vV = voltage.map(elem => elem.value);
    const cT = current.map(elem => elem.time);
    const cV = current.map(elem => elem.value);

    const dimOpts = {
        width: "500px"
    };

    return (
        <div style={{
            display: "flex",
            flexDirection: "row"
        }}>
            <div style={dimOpts}>
                <Line
                    options={{
                        scales: {
                            x: {
                                type: 'time'
                            }
                        }
                    }}
                    data={{
                        labels: vT,
                        datasets: [{
                            label: `Voltage (${type})`,
                            data: vV,
                            backgroundColor: "blue",
                            borderColor: "blue"
                        }],
                    }}
                />
            </div>

            <div style={{ ...dimOpts }}>
                <Line
                    options={{
                        scales: {
                            x: {
                                type: 'time'
                            }
                        }
                    }}
                    data={{
                        labels: cT,
                        datasets: [{
                            label: `Current (${type})`,
                            data: cV,
                            backgroundColor: "magenta",
                            borderColor: "magenta"
                        }],
                    }}
                />
            </div>
        </div>
    );
};

function App() {
    const [voltageData, setVoltageData] = useState<ValueTime[]>([]);
    const [currentData, setCurrentData] = useState<ValueTime[]>([]);
    const [error, setError] = useState<string | null>(null); // Add state for error

    useEffect(() => {
        const fetchData = async () => {
            try {
                const ddb = new DynamoDBClient({
                    region: "us-east-1",
                    credentials: {
                        accessKeyId: keys.aws_access_key_id,
                        secretAccessKey: keys.aws_secret_access_key,
                        sessionToken: keys.aws_session_token
                    }
                });

                const command = new QueryCommand({
                    TableName: "mapp_db",
                    KeyConditionExpression: "#pk = :pk", // Filter by partition key
                    ExpressionAttributeNames: { "#pk": "typ" },
                    ExpressionAttributeValues: { ":pk": { S: "grid" } },
                });
                const data = await ddb.send(command);
                const items = data.Items;

                // Extract voltage and current data from items
                const newVoltageData: ValueTime[] = items?.map((item: any) => ({
                    value: parseFloat(item?.voltage?.N || '0'), // Use optional chaining and default value to handle potential undefined or null values
                    time: parseInt(item?.time?.N || '0')
                })) || []; // Ensure a default empty array if items is undefined

                const newCurrentData: ValueTime[] = items?.map((item: any) => ({
                    value: parseFloat(item?.current?.N || '0'), // Use optional chaining and default value to handle potential undefined or null values
                    time: parseInt(item?.time?.N || '0')
                })) || []; // Ensure a default empty array if items is undefined

                // Update state with the retrieved data
                setVoltageData(newVoltageData);
                setCurrentData(newCurrentData);
                setError(null); // Clear any previous errors
            } catch (error: any) {
                console.error('Error fetching data:', error);
                setError(error.message); // Set the error message in state
            }
        };

        // Fetch data initially
        fetchData();

        // Fetch data at 5-second intervals
        const intervalId = setInterval(fetchData, 5000);

        // Clean up interval on component unmount
        return () => clearInterval(intervalId);
    }, []); // Empty dependency array to run effect only once on mount

    // Render error message if there is an error
    if (error) {
        return (
            <div>
                <h2>Error</h2>
                <p>{error}</p>
            </div>
        );
    }

    // Render the chart if there is no error
    return (
        <div>
            <Source
                voltage={voltageData}
                current={currentData}
                type='Grid'
            />
        </div>
    );
}

export default App;
