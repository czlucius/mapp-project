import { useState } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import './App.css'
import 'chartjs-adapter-moment';

import {
    Chart as ChartJS,
    ArcElement,
    Tooltip,
    Legend,
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement, Title, TimeScale
} from "chart.js";
import { Doughnut } from "react-chartjs-2";
import { Line } from 'react-chartjs-2';

import { DynamoDBClient, GetItemCommand, QueryCommand } from "@aws-sdk/client-dynamodb";
import * as keys from "./keys.json"

const ddb = new DynamoDBClient({
  region: "us-east-1",
  credentials: {
    accessKeyId: keys.aws_access_key_id,
    secretAccessKey: keys.aws_secret_access_key    
  }
});

async function a() {
  const command = new QueryCommand({
    TableName: "yourTableName",
    KeyConditionExpression: "#pk = :pk", // Filter by partition key
    ExpressionAttributeNames: { "#pk": "typ" },
    ExpressionAttributeValues: { ":pk": { S: "grid" } },
  });
  const data = await ddb.send(command);
  const items = data.Items;
  console.log(items)
  
}

interface ValueTime {
  value: number
  time: number
}

const Source = ({voltage, current, type}: {
  voltage: ValueTime[],
  current: ValueTime[],
  type: string
}) => {
  navigator.aa = a
  const vT = voltage.map(elem => {
    return elem.time
  })
  const vV = voltage.map(elem => elem.value)
  const cT = current.map(elem => elem.time)
  const cV = current.map(elem => elem.value)

  const dimOpts = {
    width:"500px"
  }
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

    <div style={{
      ...dimOpts
    }}>
      

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
  )

}

function App() {
  const [count, setCount] = useState(0)
    // ChartJS.register(
    //     CategoryScale,
    //     LinearScale,
    //     PointElement,
    //     LineElement,
    //     TimeScale,
    //     Title,
    //     Tooltip,
    //     Legend
    // );


    ChartJS.register(
        CategoryScale,
        LinearScale,
        PointElement,
        LineElement,
        TimeScale,
        Title,
        Tooltip,
        Legend
    );


  return (
    <>
      <div>
          {/*<Doughnut data={{*/}
          {/*    labels: ['Red', 'Blue', 'Yellow', 'Green', 'Purple', 'Orange'],*/}
          {/*    datasets: [{*/}
          {/*        label: '# of Votes',*/}
          {/*        data: [12, 19, 3, 5, 2, 3],*/}
          {/*        borderWidth: 1*/}
          {/*    }]*/}
          {/*}} />*/}




          <Source
            voltage={[{
              value: 5, time: 1632664468243
            }, {
              value: 4.9, time: 1642665478243
            }]}
            current={[{
              value: 5, time: 1632664468243
            }, {
              value: 4.9, time: 1642665478243
            }]}
            type='Grid'
            
            />

{/* 
          <Line
              data={{
                  labels: ['Jun', 'Jul', 'Aug'],
                  datasets: [
                      {

                          id: 1,
                          label: 'a',
                          data: [5, 6, 7],backgroundColor: "blue", borderColor: "blue"
                      },
                      {

                          id: 2,
                          label: 'b',
                          data: [7, 6, 5],
                      }
                  ],
              }}
          /> */}


        </div>
    </>
  )
}

export default App
