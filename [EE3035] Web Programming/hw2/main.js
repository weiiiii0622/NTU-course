// default setup
document.addEventListener('DOMContentLoaded', function() {
    for(var i=0; i<0; i++ ){
        addUser("Dave", "Dave");
    }
    cal();

    // time
    var updateTime = function(){
        var cur = new Date();
        var time = cur.getHours() + ':' + ('0'+cur.getMinutes()).slice(-2)+':'+('0'+cur.getSeconds()).slice(-2)+ ' | Web Programming ';
        document.getElementById('time').innerHTML = time;
    }
    setInterval(updateTime, 1000);

    //form
    // const form = document.getElementById('AddUserForm');
    // form.addEventListener('submit', (event) => {
        
    //     let photo = document.getElementById("image-file").files[0];
    //     let formData = new FormData();
        
    //     formData.append("photo", photo);
    //     console.log(formData);
    // });

}, true);

// dynamic display
var cal = function(){
    const user_wrapper = document.getElementById("user_wrapper");
    const mainuser_wrapper = document.getElementById("mainuser_wrapper");
    var usr_cnt = user_wrapper.childElementCount + mainuser_wrapper.childElementCount;
    var has_pin = mainuser_wrapper.childElementCount;
    
    if(has_pin == 0){
        if(usr_cnt == 1){
            user_wrapper.style.marginTop = "35vh";
            user_wrapper.style.gridTemplateColumns = "repeat(6, 0.9fr)";
            user_wrapper.style.gridTemplateRows = "repeat(1, 30%)";
            var e = document.querySelectorAll('.meeting_User:nth-child(1)')[0];
            e.setAttribute("style", "grid-column-start: 3; grid-column-end: 5;");
        }
        else if(usr_cnt == 2){
            user_wrapper.style.marginTop = "35vh";
            user_wrapper.style.gridTemplateColumns = "repeat(6, 0.9fr)";
            user_wrapper.style.gridTemplateRows = "repeat(1, 30%)";
            var e = document.querySelectorAll('.meeting_User:nth-child(1)')[0];
            e.setAttribute("style", "grid-column-start: 2; grid-column-end: 4;");
            var e = document.querySelectorAll('.meeting_User:nth-child(2)')[0];
            e.setAttribute("style", "grid-column-start: 4; grid-column-end: 6;");
        }
        else if(usr_cnt >= 3 && usr_cnt<= 4){
            user_wrapper.style.marginTop = "20vh";
            user_wrapper.style.gridTemplateColumns = "repeat(6, 0.9fr)";
            user_wrapper.style.gridTemplateRows = "repeat(2, 30%)";
            if(usr_cnt==3){
                var e = document.querySelectorAll('.meeting_User:nth-child(1)')[0];
                e.setAttribute("style", "grid-column-start: 2; grid-column-end: 4;");
                var e = document.querySelectorAll('.meeting_User:nth-child(2)')[0];
                e.setAttribute("style", "grid-column-start: 4; grid-column-end: 6;");
                var e = document.querySelectorAll('.meeting_User:last-child:nth-child(2n-1)')[0];
                e.setAttribute("style", "grid-column: span 4; grid-column-end: 6;");
            }
            else{
                var e = document.querySelectorAll('.meeting_User:nth-child(1)')[0];
                e.setAttribute("style", "grid-column-start: 2; grid-column-end: 4;");
                var e = document.querySelectorAll('.meeting_User:nth-child(2)')[0];
                e.setAttribute("style", "grid-column-start: 4; grid-column-end: 6;");
                var e = document.querySelectorAll('.meeting_User:nth-child(3)')[0];
                e.setAttribute("style", "grid-column-start: 2; grid-column-end: 4;");
                var e = document.querySelectorAll('.meeting_User:nth-child(4)')[0];
                e.setAttribute("style", "grid-column-start: 4; grid-column-end: 6;");
            }
        }
        else if(usr_cnt >= 5 && usr_cnt <= 6){
            user_wrapper.style.marginTop = "20vh";
            user_wrapper.style.gridTemplateColumns = "repeat(6, 0.9fr)";
            user_wrapper.style.gridTemplateRows = "repeat(2, 30%)";

            document.querySelectorAll('.meeting_User').forEach(e => {
                e.setAttribute("style", "grid-column: span 2");
            });
            if(usr_cnt==5){
                var e = document.querySelectorAll('.meeting_User:last-child:nth-child(3n-1)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 6;");
                var e = document.querySelectorAll('.meeting_User:nth-last-child(2):nth-child(3n+1)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 4;");
            }
        }
        else if(usr_cnt >= 7 && usr_cnt <= 9){
            user_wrapper.style.marginTop = "1vh";
            user_wrapper.style.gridTemplateColumns = "repeat(6, 0.9fr)";
            user_wrapper.style.gridTemplateRows = "repeat(3, 30%)";

            document.querySelectorAll('.meeting_User').forEach(e => {
                e.setAttribute("style", "grid-column: span 2");
            });
            if(usr_cnt==7){
                var e = document.querySelectorAll('.meeting_User:last-child:nth-child(3n+1)')[0];
                    e.setAttribute("style", " grid-column: span 4; grid-column-end: 6;");
            }
            else if(usr_cnt==8){
                var e = document.querySelectorAll('.meeting_User:last-child:nth-child(3n-1)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 6;");
                var e = document.querySelectorAll('.meeting_User:nth-last-child(2):nth-child(3n+1)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 4;");
            }
        }
        else if(usr_cnt >= 10 && usr_cnt <= 12){
            user_wrapper.style.marginTop = "1vh";
            user_wrapper.style.gridTemplateColumns = "repeat(8, 0.9fr)";
            user_wrapper.style.gridTemplateRows = "repeat(3, 30%)";

            document.querySelectorAll('.meeting_User').forEach(e => {
                e.setAttribute("style", "grid-column: span 2");
            });
            if(usr_cnt==10){
                var e = document.querySelectorAll('.meeting_User:last-child:nth-child(4n-2)')[0];
                e.setAttribute("style", " grid-column: span 3; grid-column-end: 8;");
                var e = document.querySelectorAll('.meeting_User:nth-last-child(2):nth-child(8n+1)')[0];
                e.setAttribute("style", " grid-column: span 3; grid-column-end: 5;");
            }
            else if(usr_cnt==11){
                var e = document.querySelectorAll('.meeting_User:nth-child(9)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 4;");
                var e = document.querySelectorAll('.meeting_User:nth-child(10)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 6;");
                var e = document.querySelectorAll('.meeting_User:nth-child(11)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 8;");
            }
        }
        else if(usr_cnt >= 13 && usr_cnt <= 15){
            user_wrapper.style.marginTop = "8vh";
            user_wrapper.style.gridTemplateColumns = "repeat(10, 0.9fr)";
            user_wrapper.style.gridTemplateRows = "repeat(3, 26.25%)";

            document.querySelectorAll('.meeting_User').forEach(e => {
                e.setAttribute("style", "grid-column: span 2");
            });
            if(usr_cnt==13){
                var e = document.querySelectorAll('.meeting_User:nth-child(11)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 5;");
                var e = document.querySelectorAll('.meeting_User:nth-child(12)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 7;");
                var e = document.querySelectorAll('.meeting_User:nth-child(13)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 9;");
            }
            else if(usr_cnt==14){
                var e = document.querySelectorAll('.meeting_User:nth-child(11)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 4;");
                var e = document.querySelectorAll('.meeting_User:nth-child(12)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 6;");
                var e = document.querySelectorAll('.meeting_User:nth-child(13)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 8;");
                var e = document.querySelectorAll('.meeting_User:nth-child(14)')[0];
                e.setAttribute("style", " grid-column: span 2; grid-column-end: 10;");
            }
        }
    }
    else{
        if(usr_cnt == 1){
            user_wrapper.style.width = "0%";
            mainuser_wrapper.style.width = "100%";
        }
        else{
            user_wrapper.style.width = "30%";
            mainuser_wrapper.style.width = "68%";
            if(usr_cnt == 2){
                user_wrapper.style.marginTop = "35vh";
                user_wrapper.style.gridTemplateColumns = "repeat(4, 0.9fr)";
                user_wrapper.style.gridTemplateRows = "repeat(1, 30%)";

                document.querySelectorAll('.meeting_User').forEach(e => {
                    e.setAttribute("style", "grid-column: span 2; grid-column-end: 4;");
                });
            }
            else if(usr_cnt == 3){
                user_wrapper.style.marginTop = "35vh";
                user_wrapper.style.gridTemplateColumns = "repeat(2, 0.9fr)";
                user_wrapper.style.gridTemplateRows = "repeat(1, 30%)";

                document.querySelectorAll('.meeting_User').forEach(e => {
                    e.setAttribute("style", "grid-column: span 1");
                });
            }
            else if(usr_cnt >= 4 && usr_cnt <= 5){
                user_wrapper.style.marginTop = "23vh";
                user_wrapper.style.gridTemplateColumns = "repeat(4, 0.9fr)";
                user_wrapper.style.gridTemplateRows = "repeat(2, 30%)";

                document.querySelectorAll('.meeting_User').forEach(e => {
                    e.setAttribute("style", "grid-column: span 2");
                });
                if(usr_cnt==4){
                    var e = document.querySelectorAll('.meeting_User:last-child:nth-child(2n-1)')[0];
                    e.setAttribute("style", " grid-column: span 2; grid-column-end: 4;");
                }
            }
            else if(usr_cnt >= 6 && usr_cnt <= 7){
                user_wrapper.style.marginTop = "20vh";
                user_wrapper.style.gridTemplateColumns = "repeat(6, 0.9fr)";
                user_wrapper.style.gridTemplateRows = "repeat(3, 20%)";

                document.querySelectorAll('.meeting_User').forEach(e => {
                    e.setAttribute("style", "grid-column: span 3");
                });
                if(usr_cnt==6){
                    var e = document.querySelectorAll('.meeting_User:last-child:nth-child(2n-1)')[0];
                    e.setAttribute("style", " grid-column: span 4; grid-column-end: 6;");
                }
            }
            else if(usr_cnt >= 8 && usr_cnt <= 9){
                user_wrapper.style.marginTop = "8vh";
                user_wrapper.style.gridTemplateColumns = "repeat(6, 0.9fr)";
                user_wrapper.style.gridTemplateRows = "repeat(4, 20%)";

                document.querySelectorAll('.meeting_User').forEach(e => {
                    e.setAttribute("style", "grid-column: span 3");
                });
                if(usr_cnt==8){
                    var e = document.querySelectorAll('.meeting_User:last-child:nth-child(2n-1)')[0];
                    e.setAttribute("style", " grid-column: span 4; grid-column-end: 6;");
                }
            }
            else if(usr_cnt == 10){
                user_wrapper.style.marginTop = "15vh";
                user_wrapper.style.gridTemplateColumns = "repeat(6, 0.9fr)";
                user_wrapper.style.gridTemplateRows = "repeat(3, 20%)";

                document.querySelectorAll('.meeting_User').forEach(e => {
                    e.setAttribute("style", "grid-column: span 2");
                });
            }
            else if(usr_cnt >= 11 && usr_cnt <= 13){
                user_wrapper.style.marginTop = "7vh";
                user_wrapper.style.gridTemplateColumns = "repeat(6, 0.9fr)";
                user_wrapper.style.gridTemplateRows = "repeat(4, 20%)";

                document.querySelectorAll('.meeting_User').forEach(e => {
                    e.setAttribute("style", "grid-column: span 2");
                });
                if(usr_cnt==11){
                    var e = document.querySelectorAll('.meeting_User:last-child:nth-child(3n+1)')[0];
                    e.setAttribute("style", " grid-column: span 4; grid-column-end: 6;");
                }
                else if(usr_cnt==12){
                    var e = document.querySelectorAll('.meeting_User:last-child:nth-child(3n-1)')[0];
                    e.setAttribute("style", " grid-column: span 3; grid-column-end: 7;");
                    var e = document.querySelectorAll('.meeting_User:nth-last-child(2):nth-child(3n+1)')[0];
                    e.setAttribute("style", " grid-column: span 3; grid-column-end: 4;");
                }
            }
            else if(usr_cnt >= 14 && usr_cnt <= 16){
                user_wrapper.style.marginTop = "8vh";
                user_wrapper.style.gridTemplateColumns = "repeat(6, 0.9fr)";
                user_wrapper.style.gridTemplateRows = "repeat(5, 15%)";

                document.querySelectorAll('.meeting_User').forEach(e => {
                    e.setAttribute("style", "grid-column: span 2");
                });
                if(usr_cnt==14){
                    var e = document.querySelectorAll('.meeting_User:last-child:nth-child(3n+1)')[0];
                    e.setAttribute("style", " grid-column: span 4; grid-column-end: 6;");
                }
                else if(usr_cnt==15){
                    var e = document.querySelectorAll('.meeting_User:last-child:nth-child(3n-1)')[0];
                    e.setAttribute("style", " grid-column: span 3; grid-column-end: 7;");
                    var e = document.querySelectorAll('.meeting_User:nth-last-child(2):nth-child(3n+1)')[0];
                    e.setAttribute("style", " grid-column: span 3; grid-column-end: 4;");
                }
            }
        }
    }

}

// pin
var pin = function(cur){
    var user = cur.parentElement.parentElement.parentElement;
    var pinned_user = document.getElementById("pin_main");
    var user_wrapper = document.getElementById("user_wrapper");
    var mainuser_wrapper = document.getElementsByClassName("meeting_MainWrapper")[0];

    if(pinned_user==null){
        user_wrapper.style.width = "30%";
        mainuser_wrapper.style.width = "68%";
        addPinnedUser(user);
        delUser(cur.parentElement);
    }
    else{
        unpin(pinned_user, 1);
        addPinnedUser(user);
        delUser(cur.parentElement);
    }
}

var unpin = function(cur, flag){
    var user = cur.parentElement.parentElement.parentElement;
    var mainuser_wrapper = user.parentElement;
    var user_wrapper = document.getElementById("user_wrapper");

    removePinnedUser(user, flag);
}

var addPinnedUser = function(user){
    const speed = 300;
    var mainuser_wrapper = document.getElementsByClassName("meeting_MainWrapper")[0];

    var MainUser = Object.assign(document.createElement("div"), 
        {
            classList: [`meeting_MainUser`],
            id: `${user.id}`,
            style: "opacity: 0",
            innerHTML: 
            `
            <div class="mute">
                <img src="media/close.png" onclick="delUser(this)" style="margin: 7px; height:50%; cursor: pointer;">
                <img src="media/muted.png" id="main_mute" style="margin: 10px; height:50%;">
            </div>
            <div class="profile">
                <div class="setting">
                    <input type="checkbox" id="pin_main" class="hide" style="height: 70%; appearance: none;" onclick="unpin(this, 0)">
                    <input type="checkbox" id="block_main" class="hide" style="appearance: none;">
                    <input type="checkbox" id="zoom_main" class="hide" style="height: 60%; appearance: none;">
                </div>
                <img src="media/${user.attributes['profile_img'].value}.png" style="height:30%; border-radius: 50%;">
            </div>
            <div class="info">
                <img src="media/pin.png" style="height:50%; border-radius: 50%; margin-left: 3%;">
                &ThickSpace;&ThickSpace;<h4 style="color: white;">${user.id}</h4>
            </div>
            `,
        }
    );
    if(user.id == 'Peanut'){
        MainUser.children[0].children[0].setAttribute("style", "display: none");
    }
    MainUser.setAttribute("profile_img", `${user.attributes['profile_img'].value}`)
    mainuser_wrapper.appendChild(MainUser);
    MainUser.style.transition = "opacity "+speed/1000+"s ease";
    setTimeout(function(){
        MainUser.style.opacity = 1;
    }, speed);
}

var removePinnedUser = function(tar, flag){
    const speed = 300;
    var mainuser_wrapper = tar.parentElement;
    var user_wrapper = document.getElementById("user_wrapper");
    
    tar.style.transition = "opacity "+speed/1000+"s ease";
    tar.style.opacity = 0;
    setTimeout(function(){
        tar.remove();
        if(flag==0){
            mainuser_wrapper.setAttribute("style", "width:0%");
            user_wrapper.setAttribute("style", "width: 98%");
        }
        console.log(tar);
        addUser(tar.id, tar.attributes['profile_img'].value);
        cal();
    }, speed);
}

// Add User
var addUser = function(name, profile_img){
    const speed = 200;
    const user_wrapper = document.getElementById("user_wrapper");
    var user = Object.assign(document.createElement("div"), 
        {
            classList: [`meeting_User-s meeting_User`],
            id: `${name}`,
            innerHTML: 
            `
            <div class="mute-sub">
            <img src="media/close.png" onclick="delUser(this)" style="margin: 7px; height:100%; cursor: pointer;">
            <img src="media/muted.png" style="margin: 7px; height:100%;">
            </div>
            <div class="profile-sub">
                <div class="setting-sub">
                    <input type="checkbox" id="pin" class="hide-sub" style="height: 70%; appearance: none;" onclick="pin(this)">
                    <input type="checkbox" id="block" class="hide-sub" style="appearance: none;">
                    <input type="checkbox" id="zoom" class="hide-sub" style="height: 60%; appearance: none;">
                </div>
                <img style="height:100%; border-radius: 50%; margin-top: -5%;">
            </div>
            <div class="info-sub">
                &ThickSpace;&ThickSpace;&ThickSpace;<h5 style="color: white;"></h5>
            </div>
            `,
        }
    );
    user.setAttribute("profile_img", `${profile_img}`)
    user.children[1].children[1].setAttribute("src", `media/${profile_img}.png`);
    user.children[2].children[0].textContent = `${name}`;
    if(user.id == 'Peanut'){
        user.children[0].children[0].setAttribute("style", "display: none");
    }
    user.style.opacity = 0;
    user_wrapper.appendChild(user);
    user.style.transition = "opacity "+speed/1000+"s ease";
    setTimeout(function() {
        user.style.opacity = 1;
        
    }, speed);
    cal();
}

var delUser = function(a){
    const speed = 500;
    var tar = a.parentElement.parentElement;
    var mainuser_wrapper = tar.parentElement;
    var user_wrapper = document.getElementById("user_wrapper");
    
    
    tar.style.transition = "opacity "+speed/1000+"s ease";
    tar.style.opacity = 0;
    setTimeout(function() {
        if(tar.classList[0]=="meeting_MainUser"){
            mainuser_wrapper.setAttribute("style", "width:0%");
            user_wrapper.setAttribute("style", "width: 98%");
        }
        tar.remove();
        cal();
    }, speed);
}

// form
var openForm = function() {
    console.log("Hii");
    document.getElementById("AddUserFormDiv").style.display = "block";
}
  
var closeForm = function() {
    document.getElementById("AddUserFormDiv").style.display = "none";
}

var submitForm = function(){
    const user_wrapper = document.getElementById("user_wrapper");
    const mainuser_wrapper = document.getElementById("mainuser_wrapper");
    var usr_cnt = user_wrapper.childElementCount + mainuser_wrapper.childElementCount;
    if(usr_cnt < 15){
        const form = document.getElementById("AddUserForm");
        var name = form.name.value;
        var profile_img = form.profile_img.value;
        addUser(name, profile_img);
        form.reset();
        closeForm();
        cal();        
    }
}



