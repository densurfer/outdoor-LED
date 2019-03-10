import { Component } from '@angular/core';
import { HttpClient } from '@angular/common/http';

@Component({
  selector: 'app-tab3',
  templateUrl: 'tab3.page.html',
  styleUrls: ['tab3.page.scss']
})
export class Tab3Page {

  home: boolean = true;
  speed: number = 2;
  data: number[] = [0,0,0]; 

  constructor(public http: HttpClient){  }

  fillLeds() {
    let url = this.getUtl();
    var msg = JSON.stringify({modus:status, speed: this.speed, fullColor: true, data: this.data});
    this.http.post(url, msg).subscribe(
      (r:any) => console.log(r), 
      (e:any) => console.log(e));
  }

  
  getUtl() {
    console.log(this.home);
    if(this.home) {
      return "http://192.168.0.165/";
    } else {
      return "http://192.168.4.1";
    }
  }

}
