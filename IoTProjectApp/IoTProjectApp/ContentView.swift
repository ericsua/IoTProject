//
//  ContentView.swift
//  IoTProjectApp
//
//  Created by Eric Suardi on 20/01/22.
//

import SwiftUI

struct ContentView: View {
    @State var datas = [APIdata]()
    @State var values = Array(repeating: 0, count: 128)
    
    
    var body: some View {
        NavigationView {
            /*
            List(datas) { book in
                Text("\(book.data)")
            }
                .padding()
                .foregroundColor(.blue)
                .padding()
                .onAppear() {
                    Api().loadData { (datas) in
                        self.datas.insert(datas, at: 0)
                    }
                    
                }.navigationTitle("MSP432 Project")*/
            
            
            
            
            let timer = Timer.scheduledTimer(withTimeInterval: 0.1, repeats: true) { timer in
                Api().loadData { (datas) in
                    self.datas.insert(datas, at: 0)
                    var i = 0
                    for d in datas.data.split(separator: ",") {
                        values[i] = Int(d) ?? 0
                        i+=1
                    }
                    ZStack {
                        
                        getPath(values: values)
                        
                        Rectangle()
                            .strokeBorder(Color.white,lineWidth: 4)
                            //.position(x: 198, y: 190)
                            .frame(width: 260, height: 260)
                        
                    }
                    
                        
                }
            }
            ZStack {
                
                getPath(values: values)
            
                Rectangle()
                    .strokeBorder(Color.white,lineWidth: 4)
                    //.position(x: 198, y: 190)
                    .frame(width: 260, height: 260)
                    
                
            }.navigationTitle("MSP432 Project")
            
        }//.navigationTitle("MSP432 Project")
    }
}

@ViewBuilder
func getPath(values: [Int]) -> some View {
        Path { path in
            path.move(to: CGPoint(x: 65, y:210))
            var idx = 0
            for i in stride(from: 66, to: 321, by: 2){
                path.addLine(to: CGPoint(x: i, y:210+(values[idx]*2)))
                path.addLine(to: CGPoint(x: i+1, y:210+(values[idx]*2)))
                idx+=1
            }
        }
        .stroke(.blue, lineWidth: 2)
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
            .preferredColorScheme(.dark)
    }
}
