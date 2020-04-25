//
//  ViewController.swift
//  BarcodeScannerApp
//
//  Created by Justin Frankle on 2/26/20.
//  Copyright © 2020 Justin Frankle. All rights reserved.
//

import UIKit
import CoreBluetooth

class ViewController: UIViewController {
    
    @IBOutlet weak var dataLabel: UILabel!
    @IBOutlet weak var descriptionLabel: UILabel!
    @IBOutlet weak var scanProptText: UILabel!
    @IBOutlet weak var scanButton: UIButton!
    @IBOutlet weak var codeBox: UITextField!
    @IBOutlet weak var imageBox: UIImageView!
    
    private var centralManager = BluetoothControl()
    
    var currentCode = "748927021356"
    let currentKey = "ojddvonn6gqxx8r60g6263p36ax6pi"
    
    var productImages = [] as [String]
    var currentImage = 0
    
    
    override func viewDidLoad() {
        super.viewDidLoad()
        dataLabel.numberOfLines = 0
        dataLabel.lineBreakMode = NSLineBreakMode.byWordWrapping
        descriptionLabel.numberOfLines = 0
        descriptionLabel.lineBreakMode = NSLineBreakMode.byWordWrapping
        //makeCall()
        // Do any additional setup after loading the view.
        
        //scan for bluetooth devices

        //centralManager.scanForPeripherals(withServices: nil, options: nil)
        
        //centralManager.stopScan()
        //currentCode = centralManager.getCurrentBarcode()
    }
    
    @IBAction func exitKeyboard(_ sender: Any) {
        self.view.endEditing(true)
    }
    
    @IBAction func codeChanged(_ sender: Any) {
        currentCode = codeBox.text!
        print("Current code is: " + currentCode)
    }
    
    @IBAction func startScan(_ sender: Any) {
        makeCall()
    }
    
    
    @IBAction func cycleImageLeft(_ sender: Any) {
        print("left")
        
        if currentImage == 0 {
            currentImage = productImages.count - 1
        }
        else {
            currentImage -= 1
        }
        
        let imageURL = URL(string: productImages[currentImage])
        
        DispatchQueue.main.async {
            self.updateImage(url: imageURL!)
        }
    }
    
    @IBAction func cycleImageRight(_ sender: Any) {
        print("right")
        
        if currentImage == productImages.count - 1 {
            currentImage = 0
        }
        else {
            currentImage += 1
        }
        
        let imageURL = URL(string: productImages[currentImage])
        
        DispatchQueue.main.async {
            self.updateImage(url: imageURL!)
        }
    }
    
    
    
    
    
    
    func updateImage (url: URL) {
        DispatchQueue.global().async { [weak self] in
            if let data = try? Data(contentsOf: url) {
                if let image = UIImage(data: data) {
                    DispatchQueue.main.async {
                        self?.imageBox.image = image
                    }
                }
            }
        }
    }
    
    func updateTitle (title: String) {
        dataLabel.text = title
    }
    
    func updateLabel (text: String, desc: String, url: URL) {
        dataLabel.text = text
        descriptionLabel.text = desc
        //scanProptText.text = "Barcode Scanned"
        
        DispatchQueue.global().async { [weak self] in
            if let data = try? Data(contentsOf: url) {
                if let image = UIImage(data: data) {
                    DispatchQueue.main.async {
                        self?.imageBox.image = image
                    }
                }
            }
        }
    }
    
    func makeCall () {
        //let todoEndpoint: String = "https://cat-fact.herokuapp.com/facts/random?animal_type=cat&amount=1"
        let todoEndpoint: String = "https://api.barcodelookup.com/v2/products?barcode=" + currentCode +  "&formatted=y&key=" + currentKey
        guard let url = URL(string: todoEndpoint) else {
          print("Error: cannot create URL")
          return
        }
        let urlRequest = URLRequest(url: url)
        
        let session = URLSession.shared
        
        let task = session.dataTask(with: urlRequest) {
          (data, response, error) in
          // check for any errors
          guard error == nil else {
            print("error calling GET on /todos/1")
            print(error!)
            return
          }
          // make sure we got data
          guard let responseData = data else {
            print("Error: did not receive data")
            return
          }
          // parse the result as JSON, since that's what the API provides
            
            //var d = Data.init()
            //let t = "tt"
            //d.append(t, count: 2)
            //print(String(decoding: responseData, as: UTF8.self))
          do {
            guard let todo = try JSONSerialization.jsonObject(with: responseData, options: .allowFragments)
              as? [String: Any] else {
              print("error trying to convert data to JSON")
              return
            }
            
            // now we have the todo
            // let's just print it to prove we can access it
            print("The todo is: " + todo.description)
            
            
            var title = ""
            var description = ""
            var imageURL = URL(string: "https://www.zerotoappstore.com/")
            if let retriever = todo["products"] as? [[String: AnyObject]] {
                for product in retriever {

                    if let field = product["product_name"] as? String {
                        title = field
                    }
                    
                    if let field = product["description"] as? String {
                        description = field
                    }
                    
                    //if let field = product[]
                    if let images = product["images"] as? [String] {
                        self.productImages = images
                        imageURL = URL(string: images[0])
                    }
                }
            }
            
            //self.dataLabel.text = todo.description
            DispatchQueue.main.async {
                // Do all your UI stuff here
                self.updateLabel(text: title, desc: description, url: imageURL!)
            }
            
            // the todo object is a dictionary
            // so we just access the title using the "title" key
            // so check for a title and print it if we have one
            guard let todoTitle = title as? String else {
              print("Could not get todo title from JSON")
              return
            }
            print("The title is: " + todoTitle)
          } catch  {
            print("error trying to convert data to JSON")
            return
          }
        }
        task.resume()
        
    }
    

    
    private class BluetoothControl: NSObject, CBCentralManagerDelegate, CBPeripheralDelegate {
        
        var btDevice: CBPeripheral! = nil
        var btServices: [CBService] = []
        var btCharacteristics: [CBCharacteristic] = []
        var centralManager: CBCentralManager!
        var currentBarcode: String!

        override init() {
            super.init()

            centralManager = CBCentralManager(delegate: self, queue: nil, options:nil)
            currentBarcode = "0"
        }
        
        func centralManagerDidUpdateState(_ central: CBCentralManager) {
            print(central.state.rawValue)
            switch (central.state) {
            case CBManagerState.poweredOff:
               print("CoreBluetooth BLE hardware is powered off")
                break;
            case CBManagerState.unauthorized:
                print("CoreBluetooth BLE state is unauthorized")
                break

            case CBManagerState.unknown:
                 print("CoreBluetooth BLE state is unknown");
                break

            case CBManagerState.poweredOn:
                print("CoreBluetooth BLE hardware is powered on and ready")
                scan()
                break;

            case CBManagerState.resetting:
                 print("CoreBluetooth BLE hardware is resetting")
                break;
            case CBManagerState.unsupported:
                  print("CoreBluetooth BLE hardware is unsupported on this platform");
                break
            @unknown default:
                print("none of the above");
            }
        }
        
        func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
            //print("peripheral found!")
            //print("name: \(peripheral.name ?? "none")")
            //print("peripheral: \(peripheral)")
            //print(advertisementData.count)
            //print(advertisementData)
            
            //ESP32 UART Test
            if(peripheral.identifier.uuidString == "1B1C6CFF-A0AD-9CF9-4EF7-6C486729C7C5") {
                print("peripheral found!")
                btDevice = peripheral
                endScan()
                centralManager.connect(btDevice, options: nil)
            }
        }
        
        // Called when it succeeded
        func centralManager(_ central: CBCentralManager,
        didConnect peripheral: CBPeripheral)
        {
            print("connected!")
            print(peripheral)
            
            btDevice.delegate = self
            btDevice.discoverServices(nil)
            
        }
        
        // Called when it failed
        func centralManager(_ central: CBCentralManager,
        didFailToConnect peripheral: CBPeripheral,
                   error: Error?)
        {
            print("failed…")
        }
        
        func scan() {
            print("Scanning for peripherals...");
            //let uuid = CBUUID(string: "1B1C6CFF-A0AD-9CF9-4EF7-6C486729C7C5")
            centralManager.scanForPeripherals(withServices: nil, options: nil)
        }
        
        func endScan() {
            print("Ending scan...")
            centralManager.stopScan()
        }
        
        //when services are found
        func peripheral(_ peripheral: CBPeripheral,
        didDiscoverServices error: Error?)
        {
            if error != nil {
                print("error: \(String(describing: error))")
                return
            }
            let services = peripheral.services
            print("Found \(services!.count) services! :\(services?.description ?? "none")")
            
            btServices = services!
            for serv in btServices {
                btDevice.discoverCharacteristics(nil, for: serv)
            }
        }
        
        //when characteristics are recieved
        func peripheral(_ peripheral: CBPeripheral,
        didDiscoverCharacteristicsFor service: CBService,
                           error: Error?)
        {
            if let error = error {
                print("error: \(error)")
                return
            }

            let characteristics = service.characteristics
            print("Found \(characteristics?.count ?? 0) characteristics!:\(characteristics?.description ?? "none")")
            
            
            btCharacteristics = characteristics!
            
            for char in btCharacteristics {
                btDevice.setNotifyValue(true, for: char)
            }
        }
        
        //called when characteristic value is updated
        func peripheral(_ peripheral: CBPeripheral,
        didUpdateValueFor characteristic: CBCharacteristic,
                    error: Error?)
        {
            if let error = error {
                print("Failed… error: \(error)")
                return
            }
        
            let data = characteristic.value! as NSData
            
            let val = String(data: data as Data, encoding: String.Encoding.utf8) ?? "Data could not be printed" //utf8.self?
            
            print("characteristic uuid: \(characteristic.uuid), value: \(val)")
        }
        
        // begins getting services
        func getServices() {
            btDevice.delegate = self
            btDevice.discoverServices(nil)
        }
        
        public func getCurrentBarcode() -> String {
            return currentBarcode
        }
        
    }
    
    
    

}

