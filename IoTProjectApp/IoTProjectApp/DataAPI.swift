//
//  DataAPI.swift
//  IoTProjectApp
//
//  Created by Eric Suardi on 22/01/22.
//

import Foundation

struct APIdata: Codable, Identifiable {
    var id: Int = 1
    var timestamp: Int
    var data: String
}

class Api : ObservableObject{
    @Published var tmpData = [APIdata]()
    
    func loadData(completion:@escaping (APIdata) -> ()) {
        guard let url = URL(string: "http://192.168.1.37:3000/api") else {
            print("Invalid url...")
            return
        }
        URLSession.shared.dataTask(with: url) { data, response, error in
            //print(data)
            let tmpData = try! JSONDecoder().decode(APIdata.self, from: data!)
            //print(tmpData)
            DispatchQueue.main.async {
                completion(tmpData)
            }
        }.resume()
        
    }
}
